This is a small demo application on how to use the lovely SQLite ORM.

At the top level, there are two third party sources:
- sqlite_orm
- tabulate (makes printing tables on the terminal prettier)

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
  - How to check & set the `user_version` SQLite pragma
- JOIN statement
- How to bind a enum directly to sqlite_orm
  - But seralize it as an integer in the DB (instead of a string)

----

How to comple:
1. Building will require that you have SQLite 3 and CMake installed.
2. In the root of the repo do:

   mkdir build/
   cd build/
   cmake ..
   make

Upon success you should see two executables.  Aptly called `todo_v1` and
`todo_v2`.  Running the v1 will either create a new "TODO database" (with schema
v1) or load one up.  And v2 will do the same (with schema v2), or migrate a
schema v1 database over to v2.

Running either of these you'll be presented with a REPL which only accepts 6
commands:
- quit  : Exits the application (You can also press Ctrl-D)
- list  : See what TODOs are in the DB
- add   : Create a new TODO item.  This has the synax of:
            add <Name> <Thing to do>
          E.g.: `add Ben Finish up the project`
- doing : Marks a TODO item as "In Progress".  Syntax:
            doing <Id no.>
          E.g.: `doing 5`
- done  : Marks a TODO item as "Done".  Synax:
            done <Id no.>
          E.g.: `done 5`
- clean : Removes all TODOs marked as "Done" from the DB.

Here's a sample run:
    $ ./todo_v2
    Loaded TODO database; Lasted edited on 2021-10-14 12:45:38 AM GMT
    > list
      Id no.   Status        Who    Task        
    --------------------------------------------
      1        In Progress   Ben    finish app  
      2        Not Started   Ben    Go to bed   
      4        Finished      John   Go to class 
    > add Amy Submit those bug reports
      Added new TODO for Amy [id=5]
    > list
      Id no.   Status        Who    Task                     
    ---------------------------------------------------------
      1        In Progress   Ben    finish app               
      2        Not Started   Ben    Go to bed                
      4        Finished      John   Go to class              
      5        Not Started   Amy    Submit those bug reports 
    > doing 5
      Marked TODO [5] as In Progress
    > done 1
      Marked TODO [1] as Finished
    > list
      Id no.   Status        Who    Task                     
    ---------------------------------------------------------
      1        Finished      Ben    finish app               
      2        Not Started   Ben    Go to bed                
      4        Finished      John   Go to class              
      5        In Progress   Amy    Submit those bug reports 
    > clean
      Removed 2 TODO(s) marked as done.
    > list
      Id no.   Status        Who   Task                     
    --------------------------------------------------------
      2        Not Started   Ben   Go to bed                
      5        In Progress   Amy   Submit those bug reports 
    > quit
    $

----

All code that I wrote is released under the "Unlicense". Please see
https://unlicense.org for details.  Third party soruces fall under
their own separate license (refer to their source code).
