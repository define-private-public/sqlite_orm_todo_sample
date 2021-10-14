#include "Storage.h"
#include <sstream>
#include <exception>
#include <filesystem>
#include <chrono>
#include "../../sqlite_orm.h"
#include "MetaData.h"
#include "TodoItem.h"

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


// Function that setups the sqlite_orm database.  It only reads in the database from disk
inline auto _create_database(const string &db_filename) {
    return make_storage(
        db_filename,
        make_table(
            "meta_data",
            make_column("key",    &MetaData::key, primary_key()),
            make_column("value",  &MetaData::value)
        ),
        make_table(
            "todo_items",
            make_column("id",     &TodoItem::id, autoincrement(), primary_key()),
            make_column("who",    &TodoItem::who),
            make_column("thing",  &TodoItem::thing),
            make_column("status", &TodoItem::get_status, &TodoItem::set_status)
        )
    );
}
using SQLiteDatabase = decltype(_create_database(""));


/** The PIMPL class for Databse access */
class Storage::_Storage {
public:
    // Data
    SQLiteDatabase db;


    _Storage(const string &db_filename) :
        db(_create_database(db_filename))
    {
        // No database on disk found, create it
        if (!fs::exists(db_filename))
            setup_new_database();

        // Read in the version no. from the DB; verify it's okay
        const int db_version_num = db.pragma.user_version();
        if (db_version_num != CurrentVersion) {
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
        db.replace(MetaData{
            .key = last_time_edited_key,
            .value = to_string(get_unix_timestamp())
        });
    }


    // Get the time that the database was last edited (as a UNIX timestamp)
    uint64_t last_time_edited_timestamp() {
        const auto last_time_edited = db.get<MetaData>(last_time_edited_key);
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


int Storage::add_todo(const string &who, const string &thing) noexcept {
    const int id = _s->db.insert(TodoItem{
        .id     = -1,       // actual id will be assigned upon succesful insertion
        .who    = who,
        .thing  = thing,
        .status = TodoItem::Status::NotStarted
    });
    _s->update_edit_time();

    return id;
}


vector<TodoItem> Storage::todos() const noexcept {
    return _s->db.get_all<TodoItem>();
}


bool Storage::change_todo_status(const int todo_id, const TodoItem::Status status) noexcept {
    // Try to get the TODO item with the given ID
    auto todo = _s->db.get_pointer<TodoItem>(todo_id);
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
    constexpr int finished_int = static_cast<int>(TodoItem::Status::Finished);  // Need to do this since `Status` is an `int` in the database
    auto status_column = c(&TodoItem::get_status);                              // Need to use the `get_` function here since that's how we created the table

    // Delete (and count how many)
    const int num_before = _s->db.count<TodoItem>();
    _s->db.remove_all<TodoItem>(where(status_column == finished_int));
    const int num_after = _s->db.count<TodoItem>();

    // If someting was delated, mark the database as updated
    const int diff = num_before - num_after;
    if (diff != 0)
        _s->update_edit_time();

    return diff;
}
