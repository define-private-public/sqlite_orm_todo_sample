#include "TodoItemRecord.h"

using namespace std;
using namespace Data::V2;


string TodoItemRecord::printable_string(const TodoItemRecord::Status s) {
    switch (s) {
        case TodoItemRecord::Status::NotStarted:    return "Not Started";
        case TodoItemRecord::Status::InProgress:    return "In Progress";
        case TodoItemRecord::Status::Finished:      return "Finished";
        default:                                    return "<UNKNOWN>";
    }
}
