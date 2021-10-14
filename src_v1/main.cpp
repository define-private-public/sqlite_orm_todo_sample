#include <iostream>
#include <string>
#include "../tabulate.hpp"
#include "Data/Storage.h"

using namespace std;
using namespace Data;


// Commands to use
const string quit_cmd("quit");
const string list_cmd("list");
const string add_cmd("add");
const string doing_cmd("doing");
const string done_cmd("done");
const string clean_cmd("clean");


// Shows a list of all TODOs in the database
void do_list_cmd(const Storage &storage) {
    // Check for no entries
    const vector<TodoItem> all_todos = storage.todos();
    if (all_todos.size() == 0) {
        cout << "  No TODOs yet..." << endl;
        return;
    }

    // Setup a nice table to display all of the TODOs
    tabulate::Table display_table;
    display_table.format().border_top("").border_bottom("").border_left("").border_right("").corner("").padding_left(2);
    display_table.add_row({"Id no.", "Status", "Who", "Task"});

    for (const TodoItem &td : all_todos) {
        display_table.add_row({
            to_string(td.id),
            TodoItem::printable_string(td.status),
            td.who,
            td.thing
        });
    }

    display_table[1].format().border_top("-");

    cout << display_table << endl;
}


// Allows users to add new todos
void do_add_cmd(Storage &storage, const string &line) {
    // parse out what we need
    const string args = line.substr(add_cmd.size() + 1);     // Rest of the line are the arguments
    const size_t pivot = args.find(' ');
    const string who = args.substr(0, pivot);               // First word is the person who needs todo it
    const string thing = args.substr(pivot + 1);            // REst of it is the task

    // Put it in
    const int id = storage.add_todo(who, thing);
    cout << "  Added new TODO for " << who << " [id=" << id << "]" << endl;
}


// Lets users change the status of a TODO
void change_status_cmd(Storage &storage, const string &line, const TodoItem::Status status) {
    // Get the id they want to change
    const string todo_id_str = line.substr(line.find(' ') + 1);
    const int todo_id = stoi(todo_id_str);

    // Try to make the change
    const bool ok = storage.change_todo_status(todo_id, status);
    if (ok)
        cout << "  Marked TODO [" << todo_id << "] as " << TodoItem::printable_string(status) << endl;
    else
        cout << "  Error, couldn't update TODO with id no. " << todo_id << endl;
}


// Cleans out all TODOs marked as "finished"
void do_clean_cmd(Storage &storage) {
    const int num_cleaned = storage.remove_finished_todos();

    if (num_cleaned != 0)
        cout << "  Removed " << num_cleaned << " TODO(s) marked as done." << endl;
    else
        cout << "  No TODOs cleaned out." << endl;
}


// Checks if a string starts with another substring
inline bool string_starts_with(const string &haystack, const string &needle)
    { return (haystack.rfind(needle, 0) == 0); }



int main(int argc, char *argv[]) {
    // Load the database
    Storage storage("todo_db.sqlite3");
    cout << "Loaded TODO database; Lasted edited on " << storage.last_time_edited() << endl;

    // Loop until EOF or `quit`
    cout << "> ";
    string line;
    while (getline(cin, line)) {
        if (line == quit_cmd)
            break;
        else if (line == list_cmd)
            do_list_cmd(storage);
        else if (string_starts_with(line, add_cmd))
            do_add_cmd(storage, line);
        else if (string_starts_with(line, doing_cmd))
            change_status_cmd(storage, line, TodoItem::Status::InProgress);
        else if (string_starts_with(line, done_cmd))
            change_status_cmd(storage, line, TodoItem::Status::Finished);
        else if (line == clean_cmd)
            do_clean_cmd(storage);

        // If not, prompt for another command
        cout << "> ";
    }
    cout << endl;

    return 0;
}
