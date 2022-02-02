//
// Created by fraggy on 15.02.21.
//

#include "util.h"

#include <SdFat.h>
#include "shared_consts.h"

const int LOGBUFSIZE = 120;
char logBuf[LOGBUFSIZE];

toml_table_t* parse_toml_from_sd(std::string const & path) {
    FatFile file;

    if (!file.open(path.c_str())) {
        SERIAL_LOG("Could not open %s", path.c_str());
        return nullptr;
    }

    char * file_buf = new char[file.fileSize() + 1];
    file_buf[file.fileSize()] = '\0';

    if(!file_buf) {
        SERIAL_LOG("Failed to allocate memory for config.");
        return nullptr;
    }

    int16_t bytes_read = file.read(file_buf, file.fileSize());

    if(bytes_read <= 0) {
        SERIAL_LOG("Failed to read config.");
        return nullptr;
    }

    toml_table_t* table = toml_parse(file_buf, logBuf, LOGBUFSIZE);

    if(!table) {
        SERIAL_LOG("Failed to parse config.");
        return nullptr;
    }

    return table;
}
