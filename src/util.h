//
// Created by fraggy on 15.02.21.
//

#ifndef COMICS_UTIL_H
#define COMICS_UTIL_H

#include <string>
#include "lib/toml.h"

/***
 * Parses a TOML file from the SD card
 *
 * Important: remember to call toml_free() on the returned pointer once you are done with it!
 *
 * @param path path to the TOML file on SD
 * @return The parsed table, or nullptr in case of error.
 */
toml_table_t* parse_toml_from_sd(std::string const & path);

#endif //COMICS_UTIL_H
