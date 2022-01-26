//
// Created by fraggy on 01.03.21.
//

#ifndef COMICS_VIEWIF_H
#define COMICS_VIEWIF_H

#include <Inkplate.h>

#include "../lib/toml.h"

/**
 * Interface that all view modules need to implement
 */
class ViewIF {
public:
    /**
     * Initializes the module
     *
     * This function is meant to do all the initialization needed by the module
     *
     * @param display Reference to the Inkplate object that allows the module to draw on the screen
     * @param cfg Pointer to the TOML table that defines this module
     */
    virtual bool setup(Inkplate &display, toml_table_t *cfg);

    /**
     * Function that gets called when the view becomes active
     *
     * This function is called when the middle button is pressed and the view gets cycled in as the active view.
     */
    virtual void resume();

    /**
     * Function that gets called when the left button is pressed
     */
    virtual void leftButton();

    /**
     * Function that gets called when the right button is pressed
     */
    virtual void rightButton();
};

#endif //COMICS_VIEWIF_H
