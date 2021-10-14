#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../TodoItem.h"


namespace Data {


/** A class that does all of the database access */
class Storage {
    // Pimpl (saves on compliation time for sqlite_orm)
    class _Storage;
    std::unique_ptr<_Storage> _s;

public:
    /** The current version of the database this program will work with. */
    static const int CurrentVersion = 2;


    /** Creates or loads a database with the given filename */
    explicit Storage(const std::string &db_filename);
    ~Storage();

    /** Returns a timestamp of when the DB was last edited */
    std::string last_time_edited() const noexcept;

    /** Add a new TODO record to the database, marked as non-started. */
    TodoItem add_todo(const std::string &who, const std::string &thing) noexcept;

    /** Get the TODOs in the database */
    std::vector<TodoItem> todos() const noexcept;

    /** Change the status of a TODO record, returns `true` upon success. */
    bool change_todo_status(const int todo_id, const TodoItem::Status status) noexcept;

    /** Removes all TODOs that have been marked as done; returns the number removed. */
    int remove_finished_todos() noexcept;
};

} // Data::
