#pragma once

#include <string>


namespace Data {
namespace V1 {

/**
 * A simple key-value store of metadata.
 */
struct MetaData {
    std::string key;                ///< The key to store data, must be unique
    std::string value;              ///< Text encodding of the value (user defined on how to decode it)
};

}   // V1::
}   // Data::
