//
// Created by fraggy on 03.01.21.
//

#include "Comics.h"
#include "../shared_consts.h"
#include "../util.h"
#include <SdFat.h>
#include <string>

Comics::Comics() {

}

Comics::~Comics() {

}

bool Comics::setup(Inkplate & display, toml_table_t * cfg) {
    m_display = &display;
    bool success = true;

    toml_datum_t dir = toml_string_in(cfg, "path");
    if(!dir.ok) {
        SERIAL_LOG("No 'path' configured for comics.");
        success = false;
    }

    if(success) {
        FatFile root;
        if(!root.open(dir.u.s)) {
            SERIAL_LOG("Could not open %s", dir.u.s);
            success = false;
        }

        m_root_str = std::string(dir.u.s);

        m_current = next(std::string(dir.u.s), false);

        std::string cached = get_last_viewed();
        if(cached != "") {
            FileType type = file_type(cached);
            if(type.isImage) {
                SERIAL_LOG("Found %s as last viewed", cached.c_str());
                m_current = cached;
            } else {
                SERIAL_LOG("'%s' doesn't exist or is not a valid image file", cached.c_str());
            }
        }
    }

    free(dir.u.s);

    return true;
}

void Comics::resume() {
    SERIAL_LOG("Comics::resume()");
    m_display->selectDisplayMode(INKPLATE_3BIT);
    m_display->clearDisplay();

    int x_offset = 0;
    int y_offset = 0;

    FileType type = file_type(m_current);

    m_display->drawImage(m_current.c_str(), type.format, Image::Position::Center, true, false);
    m_display->display();
}

void Comics::leftButton() {
    SERIAL_LOG("Comics::leftButton()");
    set_current_image(prev(m_current));
    resume();
}

void Comics::rightButton() {
    SERIAL_LOG("Comics::rightButton()");
    set_current_image(next(m_current));
    resume();
}


/************************************************
 * private
 ***********************************************/

const std::string Comics::CACHE_PATH = ".inkplate_comics.toml";
const std::string Comics::LAST_VIEWED_KEY = "last_viewed";

void Comics::set_current_image(std::string const & path) {
    m_current = path;
    set_last_viewed(path);
}

std::string Comics::next(std::string path, bool allow_ascend) {
    return get_next(path, allow_ascend, false);
}

std::string Comics::prev(std::string path, bool allow_ascend) {
    return get_next(path, allow_ascend, true);
}

std::string Comics::get_next(std::string path, bool allow_ascend, bool reverse) {
    FatFile path_obj;
    FatFile cur_dir;
    std::string cur_dir_str(path);
    std::string cur_file("");

    path_obj.open(path.c_str(), O_RDONLY);

    if(path_obj.isDir() && !allow_ascend) {
        SERIAL_LOG("%s is a directory and we're not ascending", path.c_str());
        cur_dir.open(path.c_str());
    } else {
        // if ascending is allowed and we're looking at a folder, then
        // we have just ascended from that folder - thus we *don't* want to go
        // right back into it
        SERIAL_LOG("%s is a file or a folder we ascended from", path.c_str());

        size_t last_delimiter = path.rfind('/');
        cur_file.assign(path, last_delimiter + 1, path.length() - (last_delimiter + 1));
        cur_dir_str.assign(path, 0, last_delimiter);
        cur_dir.open(cur_dir_str.c_str());
    }

    std::list<dir_entry> curDirContents = dir_contents(cur_dir);
    if(reverse) {
        curDirContents.reverse();
    }

    SERIAL_LOG("cur_dir_str: %s", cur_dir_str.c_str());
    SERIAL_LOG("cur_file: %s", cur_file.c_str());

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

            SERIAL_LOG("Next file is %s", next_file.c_str());
            FileType type = file_type(next_file);
            if(type.isImage) {
                return next_file;
            }
        } else {
            std::string next_folder(cur_dir_str);
            next_folder.append("/").append((*it).name);

            SERIAL_LOG("Next folder is %s", next_folder.c_str());

            std::string subfolder_file = get_next(next_folder, false, reverse);
            if(subfolder_file != next_folder) {
                return subfolder_file;
            }
        }

        ++it;
    }

    if(allow_ascend && cur_dir_str != m_root_str) {
        std::string ascend_next = get_next(cur_dir_str, true, reverse);
        if(ascend_next != cur_dir_str) {
            return ascend_next;
        }
    }

    return(path);
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

std::string Comics::get_last_viewed() {
    std::string last_viewed("");

    std::string cache_path = m_root_str;
    cache_path.append("/").append(CACHE_PATH);

    toml_table_t* cache = parse_toml_from_sd(cache_path);

    if(cache) {
        SERIAL_LOG("Found cache file");
        toml_datum_t toml_last_viewed = toml_string_in(cache, LAST_VIEWED_KEY.c_str());
        if(toml_last_viewed.ok) {
            SERIAL_LOG("Found key, value: %s", toml_last_viewed.u.s);
            last_viewed.assign(toml_last_viewed.u.s);
        }
    }

    return last_viewed;
}

void Comics::set_last_viewed(std::string const & path) {
    FatFile file;

    std::string cache_path = m_root_str;
    cache_path.append("/").append(CACHE_PATH);

    if(!file.open(cache_path.c_str(), FILE_WRITE)) {
        SERIAL_LOG("Failed to open %s", cache_path.c_str());
        return;
    }

    file.truncate(0);
    char buf[160];
    sprintf(buf, "%s = '%s'", LAST_VIEWED_KEY.c_str(), path.c_str());
    int bytes_written = file.write(buf);
    if(bytes_written) {
        SERIAL_LOG("Wrote %i bytes to %s", bytes_written, cache_path.c_str());
    } else {
        SERIAL_LOG("Could not write last viewed file to %s", cache_path.c_str());
    }
    file.close();
}

Comics::FileType Comics::file_type(std::string filepath) {
    FatFile file;

    file.open(filepath.c_str(), O_RDONLY);

    uint8_t header[8];

    FileType t = {
            .isImage = false,
            .format = Image::Format::BMP
    };

    if(file.read(header, 8) == 8) {
        if(header[0] == 0x42 && header[1] == 0x4d) {
            t.isImage = true;
        }
        if(header[0] == 0xff && header[1] == 0xd8 && header[2] == 0xff) {
            t.isImage = true;
            t.format = Image::Format::JPG;
        }
        if(header[0] == 0x89 && header[1] == 0x50 && header[2] == 0x4e && header[3] == 0x47
           && header[4] == 0x0d && header[5] == 0x0a && header[6] == 0x1a && header[7] == 0x0a) {
            t.isImage = true;
            t.format = Image::Format::PNG;
        }
    }

    return t;
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
