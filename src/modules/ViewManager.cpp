//
// Created by fraggy on 25/12/2021.
//

#include <cstring>
#include "ViewManager.h"
#include "Comics.h"
#include "Text.h"
#include "../shared_consts.h"
#include "../util.h"

ViewManager::ViewManager() : m_ring(), m_display(nullptr) {

}

ViewManager::~ViewManager() {

}


void ViewManager::init(Inkplate & display, toml_table_t * cfg, toml_array_t * modules_cfg) {
    m_display = &display;

    initModules(cfg, modules_cfg);
}

void ViewManager::leftButton() {
    ViewIF * curr = m_ring.getCurrent();
    if (!curr) {
        SERIAL_LOG("ViewManager::leftButton> no current view");
        return;
    }
    curr->leftButton();
}

void ViewManager::centerButton() {
    ViewIF * curr = m_ring.getNext();
    if (!curr) {
        SERIAL_LOG("ViewManager::centerButton> no current view");
        return;
    }
    SERIAL_LOG("Resuming %p.", (void *) curr);
    curr->resume();
}

void ViewManager::rightButton() {
    ViewIF * curr = m_ring.getCurrent();
    if (!curr) {
        SERIAL_LOG("ViewManager::rightButton> no current view");
        return;
    }
    curr->rightButton();
}

void ViewManager::draw() {
    ViewIF * curr = m_ring.getCurrent();
    if (!curr) {
        SERIAL_LOG("ViewManager::draw> no current view");
        return;
    }
    curr->resume();
}


// private

void ViewManager::initModules(toml_table_t * cfg, toml_array_t * modules_cfg) {
    if (!cfg) {
        SERIAL_LOG("No config found");
        while(true) {
            // nop
        }
    }
    if (!modules_cfg) {
        SERIAL_LOG("No modules found");
        while(true) {
            // nop
        }
    }

    for (int i = 0; ; i++) {
        toml_table_t * module = toml_table_at(modules_cfg, i);
        if (!module) break;

        ViewIF * view = viewBuilder(module);
        if (view) {
            m_ring.pushBack(view);
        }
    }
}


ViewIF * ViewManager::viewBuilder(toml_table_t * mod_cfg) {
    toml_datum_t type = toml_string_in(mod_cfg, "module");
    if (!type.ok) {
        SERIAL_LOG("Module has no type.");
        return nullptr;
    }

    ViewIF * newView = nullptr;
    if (strcmp("comics", type.u.s) == 0) {
        Comics * comics = new Comics();
        if (comics->setup(*m_display, mod_cfg)) {
            newView = (ViewIF *) comics;
        } else {
            SERIAL_LOG("Could not set up comics module.");
        }
    } else if (strcmp("text", type.u.s) == 0) {
        Text * text = new Text();
        if (text->setup(*m_display, mod_cfg)) {
            newView = (ViewIF *) text;
        } else {
            SERIAL_LOG("Could not set up text module.");
        }
    } else {
        SERIAL_LOG("Unknown module type '%s'.", type.u.s);
    }

    free(type.u.s);

    return newView;
}
