//
// Created by fraggy on 25/12/2021.
//

#ifndef INKPLATE_COMICS_VIEWMANAGER_H
#define INKPLATE_COMICS_VIEWMANAGER_H

#include <Inkplate.h>

#include "../lib/toml.h"
#include "ViewIF.h"
#include "ViewRing.h"

class ViewManager {
public:
    ViewManager();
    virtual ~ViewManager();

    void init(Inkplate & display, toml_array_t * modules_cfg);

    void leftButton();
    void centerButton();
    void rightButton();

    void draw();

private:
    ViewRing m_ring;
    Inkplate * m_display;

    void initModules(toml_array_t * modules_cfg);
    ViewIF * viewBuilder(toml_table_t * mod_cfg);
};


#endif //INKPLATE_COMICS_VIEWMANAGER_H
