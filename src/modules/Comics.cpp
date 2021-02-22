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
    char errBuf[ERRBUFSIZE];
    bool success = true;

    toml_table_t * table = toml_table_in(cfg, "comics");
    if(!table) {
        Serial.println("No 'comics' section in config.");
        return false;
    }

    toml_datum_t dir = toml_string_in(table, "path");
    if(!dir.ok) {
        Serial.println("No 'path' configured for comics.");
        success = false;
    }

    if(success) {
        FatFile root;
        if(!root.open(dir.u.s)) {
            sprintf(errBuf, "Could not open %s", dir.u.s);
            Serial.println(errBuf);
            success = false;
        }

        m_root_str = std::string(dir.u.s);

        m_current = next(std::string(dir.u.s), false);

        std::string cached = get_last_viewed();
        if(cached != "") {
            if(file_type(cached) == FileType::PNG) {
                sprintf(errBuf, "Found %s as last viewed", cached.c_str());
                Serial.println(errBuf);
                m_current = cached;
            } else {
                sprintf(errBuf, "'%s' doesn't exist or is not a valid image file", cached.c_str());
                Serial.println(errBuf);
            }
        }
    }

    free(dir.u.s);

    return true;
}

void Comics::resume() {
    Serial.println("Comics::resume()");
    m_display->selectDisplayMode(INKPLATE_3BIT);
    m_display->clearDisplay();

    int x_offset = 0;
    int y_offset = 0;
    char errBuf[ERRBUFSIZE];

    // centering the image
    if(file_type(m_current.c_str()) == FileType::PNG) {
        Serial.println("centering");
        reset_png_header_flag();
        pngle_t *pngle = pngle_new();
        pngle_set_init_callback(pngle, c_png_header_callback);

        FatFile file;
        file.open(m_current.c_str(), O_RDONLY);

        sprintf(errBuf, "Feeding %s into pngle", m_current.c_str());
        Serial.println(errBuf);

        uint8_t buf[16];
        size_t bytes_read = file.read(buf, 16);
        while(!png_header_is_read() && bytes_read) {
            pngle_feed(pngle, buf, bytes_read);
            bytes_read = file.read(buf, 16);
        }

        Serial.println("header read");
        sprintf(errBuf, "width: %i, height: %i", pngle_get_width(pngle), pngle_get_height(pngle));
        Serial.println(errBuf);

        if(png_header_is_read()) {
            int32_t x_margin = m_display->width() - pngle_get_width(pngle);
            if(x_margin > 0) {
                x_offset = x_margin / 2;
            }
            int32_t y_margin = m_display->height() - pngle_get_height(pngle);
            if(y_margin > 0) {
                y_offset = y_margin / 2;
            }
            sprintf(errBuf, "x_margin: %i, x_offset: %i, y_margin: %i, y_offset: %i",
                    x_margin, x_offset, y_margin, y_offset);
            Serial.println(errBuf);
        }

        pngle_destroy(pngle);
    }

    m_display->drawImage(m_current.c_str(), x_offset, y_offset, 0);
    m_display->display();
}

void Comics::leftButton() {
    Serial.println("Comics::leftButton()");
    set_current_image(prev(m_current));
    resume();
}

void Comics::rightButton() {
    Serial.println("Comics::rightButton()");
    set_current_image(next(m_current));
    resume();
}

void Comics::png_header_callback() {
    Serial.println("Comics::png_header_callback()");
    m_png_header_read = true;
}

/************************************************
 * private
 ***********************************************/

const std::string Comics::CACHE_PATH = ".inkplate_comics.toml";
const std::string Comics::LAST_VIEWED_KEY = "last_viewed";

bool Comics::m_png_header_read = false;

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
    char errBuf[ERRBUFSIZE];

    path_obj.open(path.c_str(), O_RDONLY);

    if(path_obj.isDir() && !allow_ascend) {
        sprintf(errBuf, "%s is a directory and we're not ascending", path.c_str());
        Serial.println(errBuf);
        cur_dir.open(path.c_str());
    } else {
        // if ascending is allowed and we're looking at a folder, then
        // we have just ascended from that folder - thus we *don't* want to go
        // right back into it
        sprintf(errBuf, "%s is a file or a folder we ascended from", path.c_str());
        Serial.println(errBuf);

        size_t last_delimiter = path.rfind('/');
        cur_file.assign(path, last_delimiter + 1, path.length() - (last_delimiter + 1));
        cur_dir_str.assign(path, 0, last_delimiter);
        cur_dir.open(cur_dir_str.c_str());
    }

    std::list<dir_entry> curDirContents = dir_contents(cur_dir);
    if(reverse) {
        curDirContents.reverse();
    }

    sprintf(errBuf, "cur_dir_str: %s", cur_dir_str.c_str());
    Serial.println(errBuf);
    sprintf(errBuf, "cur_file: %s", cur_file.c_str());
    Serial.println(errBuf);

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
            Serial.println(errBuf);
            // for some reason neither BMP nor JPEG get rendered properly. The screen just stays blank
            if(file_type(next_file) == FileType::PNG) {
                return next_file;
            }
        } else {
            std::string next_folder(cur_dir_str);
            next_folder.append("/").append((*it).name);

            sprintf(errBuf, "Next folder is %s", next_folder.c_str());
            Serial.println(errBuf);

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

void Comics::reset_png_header_flag() {
    m_png_header_read = false;
}

bool Comics::png_header_is_read() {
    return m_png_header_read;
}

std::string Comics::get_last_viewed() {
    std::string last_viewed("");
    char errBuf[ERRBUFSIZE];

    std::string cache_path = m_root_str;
    cache_path.append("/").append(CACHE_PATH);

    toml_table_t* cache = parse_toml_from_sd(cache_path);

    if(cache) {
        Serial.println("Found cache file");
        toml_datum_t toml_last_viewed = toml_string_in(cache, LAST_VIEWED_KEY.c_str());
        if(toml_last_viewed.ok) {
            sprintf(errBuf, "Found key, value: %s", toml_last_viewed.u.s);
            Serial.println(errBuf);
            last_viewed.assign(toml_last_viewed.u.s);
        }
    }

    return last_viewed;
}

void Comics::set_last_viewed(std::string const & path) {
    FatFile file;
    char errBuf[ERRBUFSIZE];

    std::string cache_path = m_root_str;
    cache_path.append("/").append(CACHE_PATH);

    if(!file.open(cache_path.c_str(), FILE_WRITE)) {
        sprintf(errBuf, "Failed to open %s", cache_path.c_str());
        Serial.println(errBuf);
        return;
    }

    file.truncate(0);
    char buf[160];
    sprintf(buf, "%s = '%s'", LAST_VIEWED_KEY.c_str(), path.c_str());
    int bytes_written = file.write(buf);
    if(bytes_written) {
        sprintf(errBuf, "Wrote %i bytes to %s", bytes_written, cache_path.c_str());
        Serial.println(errBuf);
    } else {
        sprintf(errBuf, "Could not write last viewed file to %s", cache_path.c_str());
        Serial.println(errBuf);
    }
    file.close();
}

Comics::FileType Comics::file_type(std::string filepath) {
    FatFile file;

    file.open(filepath.c_str(), O_RDONLY);

    uint8_t header[8];

    if(file.read(header, 8) == 8) {
        if(header[0] == 0x42 && header[1] == 0x4d) {
            return FileType::BMP;
        }
        if(header[0] == 0xff && header[1] == 0xd8 && header[2] == 0xff) {
            return FileType::JPEG;
        }
        if(header[0] == 0x89 && header[1] == 0x50 && header[2] == 0x4e && header[3] == 0x47
           && header[4] == 0x0d && header[5] == 0x0a && header[6] == 0x1a && header[7] == 0x0a) {
            return FileType::PNG;
        }
    }

    return FileType::NO_IMAGE;
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

void c_png_header_callback(pngle_t *pngle, uint32_t w, uint32_t h) {
    Serial.println("c_png_header_callback()");
    Comics::png_header_callback();
}
