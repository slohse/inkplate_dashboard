//
// Created by fraggy on 28/12/2021.
//

#include "Text.h"
#include "../util.h"

Text::Text() : m_text(""), m_fontsize(8) {

}

Text::~Text() {

}

bool Text::setup(Inkplate & display, toml_table_t * cfg) {
    m_display = &display;

    toml_datum_t cfg_text = toml_string_in(cfg, "text");
    if(!cfg_text.ok) {
        SERIAL_LOG("No 'text' configured for text.");
        return false;
    }
    m_text = cfg_text.u.s;


    toml_datum_t cfg_fontsize = toml_int_in(cfg, "size");
    if(cfg_fontsize.ok) {
        m_fontsize = cfg_fontsize.u.i;
    }

    free(cfg_text.u.s);
    return true;
}

void Text::resume() {
    SERIAL_LOG("Comics::resume()");
    m_display->selectDisplayMode(INKPLATE_1BIT);
    m_display->clearDisplay();

    m_display->setTextSize(m_fontsize);
    m_display->print(m_text.c_str());
    m_display->display();
}

void Text::leftButton() {

}

void Text::rightButton() {

}
