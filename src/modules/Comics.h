//
// Created by fraggy on 03.01.21.
//

#ifndef COMICS_COMICS_H
#define COMICS_COMICS_H

#include <Inkplate.h>
#include "../lib/toml.h"

class Comics {
public:
    Comics();
    virtual ~Comics();

    bool setup(Inkplate & display, toml_table_t * cfg);
    void dir_contents(SdFile & dir);

private:
    Inkplate * m_display;
};


#endif //COMICS_COMICS_H
