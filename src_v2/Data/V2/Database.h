#pragma once

// This file contains the database schema for version 2 of the TODO app

#include "MetaDataRecord.h"
#include "PersonRecord.h"
#include "TodoItemRecord.h"
#include "../../../sqlite_orm.h"
#include "StatusEnum_sqliteorm_serialize.h"


namespace Data {
namespace V2 {

/** Creates a SQLite ORM database with version 2 of the schema */
auto create_database(const std::string &db_filename) {
    using namespace sqlite_orm;

    return make_storage(
        db_filename,
        make_table(
            "meta_data",
            make_column("key",    &MetaDataRecord::key, primary_key()),
            make_column("value",  &MetaDataRecord::value)
        ),
        make_table(
            "todo_items",
            make_column("id",     &TodoItemRecord::id, autoincrement(), primary_key()),
            make_column("who_id", &TodoItemRecord::who_id),
            make_column("thing",  &TodoItemRecord::thing),
            make_column("status", &TodoItemRecord::status),
            foreign_key(&TodoItemRecord::who_id).references(&PersonRecord::id)
        ),
        make_table(
            "persons",
            make_column("id",     &PersonRecord::id, autoincrement(), primary_key()),
            make_column("name",   &PersonRecord::name)
        )
    );
}
using SQLiteDatabase = decltype(create_database(""));

}   // V2::
}   // Data::
