#pragma once

// This file contains the database schema for version 1 of the TODO app

#include "MetaData.h"
#include "TodoItem.h"
#include "../../../sqlite_orm.h"


namespace Data {
namespace V1 {

/** Creates a SQLite ORM database with version 1 of the schema */
auto create_database(const std::string &db_filename) {
    using namespace sqlite_orm;

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
using SQLiteDatabase = decltype(create_database(""));

}   // V1::
}   // Data::
