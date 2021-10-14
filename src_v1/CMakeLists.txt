# Make sure executable goes into top-level build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# This is a C++17 project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SQLITE_ORM_ENABLE_CXX_17 ON)

# Since we are including sqlite_orm directly in this repo, need to make sure SQLite is installed
find_package(SQLite3 REQUIRED)

# Set the executable
add_executable(todo_v1)

# Grab the source code
target_sources(todo_v1 PRIVATE
  main.cpp
  Data/Storage.cpp
  Data/TodoItem.cpp
)

# Link it
target_link_libraries(todo_v1 PRIVATE SQLite::SQLite3)
