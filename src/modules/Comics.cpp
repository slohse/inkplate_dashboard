//
// Created by fraggy on 03.01.21.
//

#include "Comics.h"
#include "../shared_consts.h"
#include <SdFat.h>
#include <string>
#include <vector>

Comics::Comics() {

}

Comics::~Comics() {

}

bool Comics::setup(Inkplate & display, toml_table_t * cfg) {
    m_display = &display;
    char errBuf[ERRBUFSIZE];
    bool success = true;

    toml_table_t * table = toml_table_in(cfg, "comics");
    if(!table) {
        m_display->println("No 'comics' section in config.");
        m_display->partialUpdate();
        return false;
    }

    toml_datum_t dir = toml_string_in(table, "path");
    if(!dir.ok) {
        m_display->println("No 'path' configured for comics.");
        m_display->partialUpdate();
        success = false;
    }

    if(success) {
        if(!m_root.open(dir.u.s)) {
            sprintf(errBuf, "Could not open %s", dir.u.s);
            m_display->println(errBuf);
            m_display->partialUpdate();
            success = false;
        }

        next(std::string(dir.u.s));
    }

    free(dir.u.s);

    return true;
}


void Comics::next(std::string path) {
    FatFile path_obj;
    FatFile cur_dir;
    std::string cur_file("");
    char errBuf[ERRBUFSIZE];

    path_obj.open(path.c_str(), O_RDONLY);

    if(path_obj.isFile()) {
        sprintf(errBuf, "%s is a file", path.c_str());
        m_display->println(errBuf);
        m_display->partialUpdate();

        size_t last_delimiter = path.rfind('/');
        cur_file.assign(path, last_delimiter + 1, path.length() - (last_delimiter + 1));
        std::string dir(path, 0, last_delimiter - 1);
        cur_dir.open(dir.c_str());
    }

    if(path_obj.isDir()) {
        sprintf(errBuf, "%s is a directory", path.c_str());
        m_display->println(errBuf);
        m_display->partialUpdate();

        cur_dir.open(path.c_str());
    }

    std::list<dir_entry> curDirContents = dir_contents(cur_dir);

    if

    for(std::list<dir_entry>::iterator it = curDirContents.begin(); it != curDirContents.end(); ++it) {
        m_display->println((*it).name.c_str());
    }
    m_display->partialUpdate();
}

std::list<Comics::dir_entry> Comics::dir_contents(FatFile & dir) {
    FatFile file;
    char buf[80];

    std::list<dir_entry> dirContents;

    dir.rewind();

    while(file.openNext(&dir, O_RDONLY)) {
        file.getName(buf, 80);
        dir_entry entry;
        entry.isDir = file.isDir();
        entry.name = std::string(buf);
        dirContents.push_back(entry);
        file.close();
    }

    dirContents.sort(compare_dir_entry);

    return dirContents;
}

/************************************************
 * private
 ***********************************************/

bool Comics::compare_dir_entry(const dir_entry& first, const dir_entry& second) {
    if(first.isDir && !second.isDir) {
        return true;
    }
    if(second.isDir && !first.isDir) {
        return false;
    }
    return first.name < second.name;
}
