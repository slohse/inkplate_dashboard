//
// Created by fraggy on 15.02.21.
//

#include "util.h"

#include <SdFat.h>
#include "shared_consts.h"

toml_table_t* parse_toml_from_sd(std::string const & path) {
    FatFile file;
    char errBuf[ERRBUFSIZE];

    if (!file.open(path.c_str())) {
        sprintf(errBuf, "Could not open %s", path.c_str());
        Serial.println(errBuf);
        return nullptr;
    }

    char * file_buf = new char[file.fileSize() + 1];
    file_buf[file.fileSize()] = '\0';

    if(!file_buf) {
        Serial.println("Failed to allocate memory for config.");
        return nullptr;
    }

    int16_t bytes_read = file.read(file_buf, file.fileSize());

    if(bytes_read <= 0) {
        Serial.println("Failed to read config.");
        return nullptr;
    }

    toml_table_t* table = toml_parse(file_buf, errBuf, ERRBUFSIZE);

    if(!table) {
        Serial.println("Failed to parse config.");
        return nullptr;
    }

    return table;
}
