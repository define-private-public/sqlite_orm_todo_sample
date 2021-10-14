#pragma once

#include <string>
#include <memory>


namespace Data {
namespace V2 {

struct TodoItemRecord {
    // Enum to mark the status of the item
    enum class Status {
        NotStarted = 1,
        InProgress = 2,
        Finished   = 3,
    };


    // Data
    int id;                     ///< Unique identifier for the task
    int who_id;                 ///< Id no. of a person who needs to complete the task
    std::string thing;          ///< Things that needs to be done
    Status status;              ///< How far along the item is in being done

    /** Get a string that's human friendly */
    static std::string printable_string(const Status s);
};

} // V2::
} // Data::
