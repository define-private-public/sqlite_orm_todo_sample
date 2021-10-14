#include "TodoItem.h"

using namespace std;
using namespace Data::V1;


string TodoItem::printable_string(const TodoItem::Status s) {
    switch (s) {
        case TodoItem::Status::NotStarted:  return "Not Started";
        case TodoItem::Status::InProgress:  return "In Progress";
        case TodoItem::Status::Finished:    return "Finished";
        default:                            return "<UNKNOWN>";
    }
}
