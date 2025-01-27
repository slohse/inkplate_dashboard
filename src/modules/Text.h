//
// Created by fraggy on 28/12/2021.
//

#ifndef INKPLATE_COMICS_TEXT_H
#define INKPLATE_COMICS_TEXT_H


#include <string>
#include <Inkplate.h>
#include "ViewIF.h"

class Text : public ViewIF {
public:
    Text();
    virtual ~Text();

    bool setup(Inkplate & display, toml_table_t * cfg);
    void resume();
    void leftButton();
    void rightButton();

private:
    Inkplate * m_display;
    std::string m_text;
    int64_t m_fontsize;
    int16_t m_cursor_x;
    int16_t m_cursor_y;
};


#endif //INKPLATE_COMICS_TEXT_H
