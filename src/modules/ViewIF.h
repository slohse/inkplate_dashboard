//
// Created by fraggy on 01.03.21.
//

#ifndef COMICS_VIEWIF_H
#define COMICS_VIEWIF_H

#include "../lib/toml.h"

class ViewIF {
public:
    virtual bool setup(Inkplate &display, toml_table_t *cfg);

    virtual void resume();

    virtual void leftButton();

    virtual void rightButton();
};

#endif //COMICS_VIEWIF_H
