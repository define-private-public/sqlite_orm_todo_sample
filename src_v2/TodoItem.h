#pragma once

#include "Data/V2/TodoItemRecord.h"


/**
 * A thing TODO.
 *
 * In the context of this project, it may seem a bit unecessary, but keep in mind
 * these purposes are fore education.  This is meant to provide an abstraction over
 * the database record.
 *
 * In the real world, this class might do even more.  Possibly having a reference
 * to the `Storage` class itself; so that a programmer could modify this object
 * instead of having to interact with the `Storage` class directly.
 **/
class TodoItem {
    // Data
    Data::V2::TodoItemRecord _record;           ///< The backing DB record
    std::string _who_name;                      ///< Name of the person who needs TODO the thing

public:
    // "Forward" the status enum from the DB object
    using Status = Data::V2::TodoItemRecord::Status;

    TodoItem(const Data::V2::TodoItemRecord &rec, const std::string &who) :
        _record(rec),
        _who_name(who)
    { }

    /** The Id of the TODO in the DB */
    inline int id() const noexcept
        { return _record.id; }

    /** Name of the person who needs to complete the TODO */
    inline std::string who() const noexcept
        { return _who_name; }

    /** The exact task that needs to be done */
    inline std::string thing() const noexcept
        { return _record.thing; }

    /** The current status of the TODO, as a printable string */
    inline std::string status_as_printable_string() const noexcept
        { return Data::V2::TodoItemRecord::printable_string(_record.status); }

    inline static std::string printable_string(const Status s) noexcept
        { return Data::V2::TodoItemRecord::printable_string(s); }
};
