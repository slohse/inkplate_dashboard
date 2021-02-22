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
#include <libs/pngle/pngle.h>

class Comics {
public:
    Comics();
    virtual ~Comics();

    bool setup(Inkplate & display, toml_table_t * cfg);
    void resume();
    void leftButton();
    void rightButton();

    static void png_header_callback();

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

    static const std::string CACHE_PATH;
    static const std::string LAST_VIEWED_KEY;

    Inkplate * m_display;

    std::string m_root_str;
    std::string m_current;

    static bool m_png_header_read;

    void set_current_image(std::string const & path);

    std::string next(std::string path, bool allow_ascend = true);
    std::string prev(std::string path, bool allow_ascend = true);

    std::string get_next(std::string path, bool allow_ascend = true, bool reverse = false);

    std::list<dir_entry> dir_contents(FatFile & dir);

    void reset_png_header_flag();
    bool png_header_is_read();

    std::string get_last_viewed();
    void set_last_viewed(std::string const & path);

    static FileType file_type(std::string filepath);
    static bool compare_dir_entry(const dir_entry& first, const dir_entry& second);
};

#ifdef __cplusplus
extern "C" {
#endif
void c_png_header_callback(pngle_t *pngle, uint32_t w, uint32_t h);
#ifdef __cplusplus
}
#endif


#endif //COMICS_COMICS_H
