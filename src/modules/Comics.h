//
// Created by fraggy on 03.01.21.
//

#ifndef COMICS_COMICS_H
#define COMICS_COMICS_H

#include <string>
#include <list>
#include <Inkplate.h>
#include <SdFat.h>
#include "../lib/toml.h"

class Comics {
public:
    Comics();
    virtual ~Comics();

    bool setup(Inkplate & display, toml_table_t * cfg);
    void resume();
    void leftButton();
    void rightButton();

private:
    typedef struct {
        bool isDir;
        std::string name;
    } dir_entry;

    enum class FileType {
        NO_IMAGE,
        BMP,
        JPEG,
        PNG
    };

    Inkplate * m_display;

    std::string m_root_str;
    std::string m_current;

    std::list<dir_entry> dir_contents(FatFile & dir);
    std::string next(std::string path, bool allow_ascend = true);
    std::string prev(std::string path, bool allow_ascend = true);

    std::string get_next(std::string path, bool allow_ascend = true, bool reverse = false);

    static FileType file_type(std::string filepath);
    static bool compare_dir_entry(const dir_entry& first, const dir_entry& second);
};


#endif //COMICS_COMICS_H
