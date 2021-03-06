//
// Created by fraggy on 03.01.21.
//

#ifndef COMICS_COMICS_H
#define COMICS_COMICS_H

#include <string>
#include <list>
#include <Inkplate.h>
#include <SdFat.h>
#include "ViewIF.h"
#include <include/Image.h>

class Comics : public ViewIF {
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

    typedef struct {
        bool isImage;
        Image::Format format;
    } FileType;

    static const std::string CACHE_PATH;
    static const std::string LAST_VIEWED_KEY;

    Inkplate * m_display;

    std::string m_root_str;
    std::string m_current;

    void set_current_image(std::string const & path);

    std::string next(std::string path, bool allow_ascend = true);
    std::string prev(std::string path, bool allow_ascend = true);

    std::string get_next(std::string path, bool allow_ascend = true, bool reverse = false);

    std::list<dir_entry> dir_contents(FatFile & dir);

    std::string get_last_viewed();
    void set_last_viewed(std::string const & path);

    static FileType file_type(std::string filepath);
    static bool compare_dir_entry(const dir_entry& first, const dir_entry& second);
};


#endif //COMICS_COMICS_H
