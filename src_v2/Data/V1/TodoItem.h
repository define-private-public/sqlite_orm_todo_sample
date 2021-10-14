#pragma once

#include <string>


namespace Data {
namespace V1 {

struct TodoItem {
    // Enum to mark the status of the item
    enum class Status {
        NotStarted = 1,
        InProgress = 2,
        Finished   = 3,
    };


    // Data
    int id;                 ///< Unique identifier for the task
    std::string who;        ///< Name of person who is doing it
    std::string thing;      ///< Things that needs to be done
    Status status;          ///< How far along the item is in being done


    // Getter/Setter for `Status` seriaization to the DB
    int get_status() const
        { return static_cast<int>(status); }
    void set_status(const int s)
        { status = static_cast<Status>(s); }


    /** Get a string that's human friendly */
    static std::string printable_string(const Status s);
};

} // V1::
} // Data::
