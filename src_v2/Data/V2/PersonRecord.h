#pragma once

#include <string>


namespace Data {
namespace V2 {

struct PersonRecord {
    // Data
    int id;             ///< Unique Id for the person
    std::string name;   ///< Name of the person (should be name with no spaces)
};

} // V2::
} // Data::
