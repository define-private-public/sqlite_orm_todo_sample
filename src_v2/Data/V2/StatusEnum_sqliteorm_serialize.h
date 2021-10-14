#pragma once

// This is an example of how to serialize an enumeration (as an int) into a sqlite_orm
// friendly format.  The official example here only uses strings:
//
//     https://github.com/fnc12/sqlite_orm/blob/1ee0a8653fe57ed4d4f69b5a65839b1861c41d32/examples/enum_binding.cpp
//
// Note that this code could be put into a template or a preprocessor macro function to make things easier


#include "../../../sqlite_orm.h"
#include "TodoItemRecord.h"

namespace sqlite_orm {
    // We want our enums saved as INTEGER types in sqlite, so we need to derive from
    // `integer_printer`
    template<>
    struct type_printer<Data::V2::TodoItemRecord::Status> : public integer_printer {};

    template<>
    struct statement_binder<Data::V2::TodoItemRecord::Status> {
        int bind(
            sqlite3_stmt *stmt,
            int index,
            const Data::V2::TodoItemRecord::Status &value
        ) {
            return statement_binder<int>().bind(stmt, index, static_cast<int>(value));
        }
    };

    template<>
    struct field_printer<Data::V2::TodoItemRecord::Status> {
        int operator()(const Data::V2::TodoItemRecord::Status &t) {
            return static_cast<int>(t);
        }
    };

    template<>
    struct row_extractor<Data::V2::TodoItemRecord::Status> {
        Data::V2::TodoItemRecord::Status extract(const int row_value) {
            return static_cast<Data::V2::TodoItemRecord::Status>(row_value);
        }

        Data::V2::TodoItemRecord::Status extract(sqlite3_stmt *stmt, int column_index) {
            auto i = sqlite3_column_int(stmt, column_index);
            return this->extract(i);
        }
    };
}
