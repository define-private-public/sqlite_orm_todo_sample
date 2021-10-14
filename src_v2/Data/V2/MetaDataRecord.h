#pragma once

#include <string>


namespace Data {
namespace V2 {

/**
 * A simple key-value store of metadata.
 */
struct MetaDataRecord {
    std::string key;                ///< The key to store data, must be unique
    std::string value;              ///< Text encodding of the value (user defined on how to decode it)
};

}   // V2::
}   // Data::
