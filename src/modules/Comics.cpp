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


/************************************************
 * private
 ***********************************************/

const std::list<std::string> Comics::image_file_extensions({".jpg", ".jpeg", ".png", ".bmp"});

std::string Comics::next(std::string path, bool allow_ascend) {
    FatFile path_obj;
    FatFile cur_dir;
    std::string cur_dir_str(path);
    std::string cur_file("");
    char errBuf[ERRBUFSIZE];

    path_obj.open(path.c_str(), O_RDONLY);

    if(path_obj.isFile()) {
        sprintf(errBuf, "%s is a file", path.c_str());
        m_display->println(errBuf);
        m_display->partialUpdate();

        size_t last_delimiter = path.rfind('/');
        cur_file.assign(path, last_delimiter + 1, path.length() - (last_delimiter + 1));
        cur_dir_str.assign(path, 0, last_delimiter - 1);
        cur_dir.open(cur_dir_str.c_str());
    }

    if(path_obj.isDir()) {
        sprintf(errBuf, "%s is a directory", path.c_str());
        m_display->println(errBuf);
        m_display->partialUpdate();

        cur_dir.open(path.c_str());
    }

    std::list<dir_entry> curDirContents = dir_contents(cur_dir);

    std::list<dir_entry>::iterator it = curDirContents.begin();
    if(cur_file != "") {
        while((*it).name != cur_file) {
            ++it;
        }
        ++it;
    }

    while(it != curDirContents.end()) {
        if(!(*it).isDir) {
            std::string next_file(cur_dir_str);
            next_file.append("/").append((*it).name);

            sprintf(errBuf, "Next file is %s", next_file.c_str());
            m_display->println(errBuf);
            m_display->partialUpdate();
            if(is_image_file(next_file)) {
                return next_file;
            }
        } else {
            std::string next_folder(cur_dir_str);
            next_folder.append("/").append((*it).name);

            sprintf(errBuf, "Next folder is %s", next_folder.c_str());
            m_display->println(errBuf);
            m_display->partialUpdate();

            std::string subfolder_file = next(next_folder, false);
            if(subfolder_file != "") {
                return subfolder_file;
            }
        }

        ++it;
    }

    // TODO: ascend if allowed, otherwise: no file found... did we reach the end?

    m_display->partialUpdate();
    return(std::string(""));
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

bool Comics::is_image_file(std::string file) {
    // TODO: case insensitivity
    for(std::list<std::string>::const_iterator it = image_file_extensions.begin();
        it != image_file_extensions.end();
        ++it) {
        if(file.find((*it), file.length() - (*it).length()) != std::string::npos) {
            return true;
        }
    }

    return false;
}

bool Comics::compare_dir_entry(const dir_entry& first, const dir_entry& second) {
    if(first.isDir && !second.isDir) {
        return true;
    }
    if(second.isDir && !first.isDir) {
        return false;
    }
    return first.name < second.name;
}
