This is a small demo application on how to use the lovely SQLite ORM.

At the top level, there are two third party sources:
- sqlite_orm
- tabulate (makes printing tables on the terminal prettier)

Building will require that you have SQLite 3 and CMake installed.

To show off how migrations could work, there is a `src_v1/` and `src_v2/`
projects.  There is a bit of code duplicaiton between the two, but please
bear with it (for educational purposes).  Note that at times this can seem
a bit overenginered for a very simple tasks; but this is to also show off
some possible design patterns (e.g. abstracting DB access/records behind
other objects).

The stuff you may care about are in the `Data/` folders, along with the
`Storage` classes.

Things v1 shows off:
- Hiding SQLite ORM behind a `Storage` class (e.g. PIMPL principle)
- Simple CRUD operations
- Serialize enums as integers in the database (pretty simple)
- Count rows in a table
- Deleting items with a TODO clause

Things v2 shows off:
- Data migration
- JOIN statement
- How to bind a enum directly to sqlite_orm
  - But seralize it as an integer in the DB (instead of a string)

----

All code that I wrote is released under the "Unlicense". Please see
https://unlicense.org for details.  Third party soruces fall under
their own separate license (refer to their source code).
