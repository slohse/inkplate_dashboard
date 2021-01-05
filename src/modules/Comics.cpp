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
    char errbuf[ERRBUFSIZE];
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
        FatFile comics_dir;

        if(!comics_dir.open(dir.u.s)) {
            sprintf(errbuf, "Could not open %s", dir.u.s);
            m_display->println(errbuf);
            m_display->partialUpdate();
            success = false;
        }

        dir_contents(comics_dir);
    }

    free(dir.u.s);

    return true;
}

void Comics::dir_contents(FatFile & dir) {
    FatFile file;
    char buf[80];

    m_curDirContents.clear();

    dir.getName(buf, 80);

    dir.rewind();

    while(file.openNext(&dir, O_RDONLY)) {
        file.getName(buf, 80);
        dir_entry entry;
        entry.isDir = file.isDir();
        entry.name = std::string(buf);
        m_curDirContents.push_back(entry);
        file.close();
    }

    sprintf(buf, "found %i entries in folder", m_curDirContents.size());
    m_display->println(buf);
    m_display->partialUpdate();

    for (std::list<dir_entry>::iterator it = m_curDirContents.begin(); it != m_curDirContents.end(); ++it) {
        m_display->println((*it).name.c_str());
    }
    m_display->partialUpdate();
}
