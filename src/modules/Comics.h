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

private:
    typedef struct {
        bool isDir;
        std::string name;
    } dir_entry;

    Inkplate * m_display;

    std::string m_root_str;

    std::list<dir_entry> dir_contents(FatFile & dir);
    std::string next(std::string path, bool allow_ascend = true);

    static bool is_image_file(std::string filepath);
    static bool compare_dir_entry(const dir_entry& first, const dir_entry& second);
};


#endif //COMICS_COMICS_H
