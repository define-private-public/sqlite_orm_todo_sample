#include "Storage.h"
#include <sstream>
#include <exception>
#include <filesystem>
#include <chrono>
#include "V1/Database.h"
#include "V2/Database.h"


using namespace std;
using namespace std::chrono;
using namespace sqlite_orm;
using namespace Data;
namespace fs = std::filesystem;


// Constants
constexpr char last_time_edited_key[] = "last_time_edited_timestamp";



// Simple function to get the current Unix timestamp
inline uint64_t get_unix_timestamp() {
    const auto secs = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    return static_cast<uint64_t>(secs);
}


/** The PIMPL class for Databse access */
class Storage::_Storage {
public:
    // Data
    V2::SQLiteDatabase db;


    _Storage(const string &db_filename) :
        db(V2::create_database(db_filename))
    {
        // No database on disk found, create it
        if (!fs::exists(db_filename))
            setup_new_database();

        // Read in the version no. from the DB; verify it's okay
        const int db_version_num = db.pragma.user_version();
        if (db_version_num == 1) {
            // We're loading an older database, need to migrate the data

            // First make a copy of the db file
            stringstream backup_name;
            backup_name << db_filename << ".v1_bak";
            const bool backed_up = fs::copy_file(db_filename, backup_name.str());

            if (!backed_up)
                throw runtime_error("Wasn't able to backup database when migrating from v1 to v2");

            // Update the schema of the new one (and the data)
            db.pragma.user_version(CurrentVersion);
            db.sync_schema();
            update_edit_time();
            migrate_v1_to_v2(backup_name.str());
        } else if (db_version_num > CurrentVersion) {
            // The version no. isn't want we want, throw an error message
            stringstream err_msg;
            err_msg << "Invalid DB version no. = " << db_version_num << "; ";
            err_msg << "expected = " << CurrentVersion;

            throw invalid_argument(err_msg.str());
        }
    }


    void setup_new_database() {
        // Saves the skeleton database to the disk
        db.pragma.user_version(CurrentVersion);
        db.sync_schema();
        update_edit_time();
    }


    // Marks when the database was last updated
    void update_edit_time() {
        db.replace(V2::MetaDataRecord{
            .key = last_time_edited_key,
            .value = to_string(get_unix_timestamp())
        });
    }


    // Migrates the data from a v1 database over to v2
    // This funciton should only be called in the constructor
    void migrate_v1_to_v2(const string &v1_db_filepath) {
        // Need to load the backup to migrate over data
        V1::SQLiteDatabase db_v1 = V1::create_database(v1_db_filepath);

        // Go through each old TODO, and re-insert it into the database
        const auto old_todos = db_v1.get_all<V1::TodoItem>();
        for (const V1::TodoItem &old_todo : old_todos) {
            // First need to create (or retrive a person's id)
            const int who_id = get_id_or_create_person(old_todo.who);

            // Add the todo (copy over old data)
            const V2::TodoItemRecord new_todo{
                .id     = old_todo.id,
                .who_id = who_id,
                .thing  = old_todo.thing,
                .status = status_v1_to_v2(old_todo.status)
            };
            db.replace(new_todo);
        }

        update_edit_time();
    }

    // Will get the `id` of the PersonRecord with the given name
    // If that person doesn't exist, then it will insert one and then return that id
    int get_id_or_create_person(const string &name) {
        using namespace Data::V2;

        int who_id = -1;
        auto person_ids = db.select(&PersonRecord::id, where(c(&PersonRecord::name) == name));
            if (person_ids.empty()) {
                // Need to insert a new person
                who_id = db.insert(PersonRecord{
                    .id   = -1,
                    .name = name
                });
                update_edit_time();
            } else
                who_id = person_ids[0];     // There's an existing person with this name

        return who_id;
    }

    // Maps a V1's Status enum to that of V2's.
    // Note that these are the same, but in a more complex application, this could well,
    //   require something more complex.
    V2::TodoItemRecord::Status status_v1_to_v2(const V1::TodoItem::Status s) {
        switch (s) {
            case V1::TodoItem::Status::NotStarted:  return V2::TodoItemRecord::Status::NotStarted;
            case V1::TodoItem::Status::InProgress:  return V2::TodoItemRecord::Status::InProgress;
            case V1::TodoItem::Status::Finished:    return V2::TodoItemRecord::Status::Finished;
        }

        return V2::TodoItemRecord::Status::NotStarted;
    }


    // Get the time that the database was last edited (as a UNIX timestamp)
    uint64_t last_time_edited_timestamp() {
        const auto last_time_edited = db.get<V2::MetaDataRecord>(last_time_edited_key);
        return stoull(last_time_edited.value);
    }
};



// == The public facing class

Storage::Storage(const std::string &db_filename) :
    _s(make_unique<_Storage>(db_filename))
{ }


// Required for automatic unique pointer cleanup
Storage::~Storage() = default;


string Storage::last_time_edited() const noexcept {
    // Convert the time to a string
    const time_t t = _s->last_time_edited_timestamp();
    tm *timestamp_str = gmtime(&t);

    // Format and return it
    stringstream ss;
    ss << put_time(timestamp_str, "%Y-%m-%d %I:%M:%S %p GMT");

    return ss.str();
}


TodoItem Storage::add_todo(const string &who, const string &thing) noexcept {
    using namespace Data::V2;

    // Add the new TODO
    TodoItemRecord todo_rec {
        .id     = -1,
        .who_id = _s->get_id_or_create_person(who),
        .thing  = thing,
        .status = TodoItem::Status::NotStarted
    };
    const int todo_id = _s->db.insert(todo_rec);
    _s->update_edit_time();

    // Return the result
    todo_rec.id = todo_id;
    return TodoItem(todo_rec, who);
}


vector<TodoItem> Storage::todos() const noexcept {
    using namespace Data::V2;

    // Retrive all TODOs, and join the "person" records names
    auto rows = _s->db.select(
        columns(
            &TodoItemRecord::id,
            &TodoItemRecord::who_id,
            &TodoItemRecord::thing,
            &TodoItemRecord::status,
            &PersonRecord::name
        ),
        join<PersonRecord>(on(
            c(&TodoItemRecord::who_id) == &PersonRecord::id
        ))
    );

    // Stuff it into a records
    vector<TodoItem> all_todos;
    for (const auto &row : rows) {
        all_todos.push_back(TodoItem(
            TodoItemRecord{
                .id     = get<0>(row),
                .who_id = get<1>(row),
                .thing  = get<2>(row),
                .status = static_cast<TodoItemRecord::Status>(get<3>(row))
            },
            get<4>(row)
        ));
    }

    return all_todos;
}



bool Storage::change_todo_status(const int todo_id, const TodoItem::Status status) noexcept {
    using namespace Data::V2;

    // Try to get the TODO item with the given ID
    auto todo = _s->db.get_pointer<TodoItemRecord>(todo_id);
    if (todo) {
        // If so, update it
        todo->status = status;
        _s->db.update(*todo);
        _s->update_edit_time();
        return true;
    }

    // Couldn't find the item, return false for failure
    return false;
}


int Storage::remove_finished_todos() noexcept {
    using namespace Data::V2;

    constexpr int finished_int = static_cast<int>(TodoItemRecord::Status::Finished);    // Need to do this since `Status` is an `int` in the database
    auto status_column = c(&TodoItemRecord::status);

    // Delete (and count how many)
    const int num_before = _s->db.count<TodoItemRecord>();
    _s->db.remove_all<TodoItemRecord>(where(status_column == finished_int));
    const int num_after = _s->db.count<TodoItemRecord>();

    // If someting was delated, mark the database as updated
    const int diff = num_before - num_after;
    if (diff != 0)
        _s->update_edit_time();

    return diff;
}
