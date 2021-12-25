//
// Created by fraggy on 25/12/2021.
//

#include <cstring>
#include "ViewManager.h"
#include "Comics.h"
#include "../shared_consts.h"

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
        Serial.println("ViewManager::leftButton> no current view");
        return;
    }
    curr->leftButton();
}

void ViewManager::centerButton() {
    ViewIF * curr = m_ring.getNext();
    if (!curr) {
        Serial.println("ViewManager::centerButton> no current view");
        return;
    }
    curr->resume();
}

void ViewManager::rightButton() {
    ViewIF * curr = m_ring.getCurrent();
    if (!curr) {
        Serial.println("ViewManager::rightButton> no current view");
        return;
    }
    curr->rightButton();
}

void ViewManager::draw() {
    ViewIF * curr = m_ring.getCurrent();
    if (!curr) {
        Serial.println("ViewManager::draw> no current view");
        return;
    }
    curr->resume();
}


// private

void ViewManager::initModules(toml_table_t * cfg, toml_array_t * modules_cfg) {
    char errBuf[ERRBUFSIZE];

    if (!cfg) {
        Serial.println("No config found");
        while(true) {
            // nop
        }
    }
    if (!modules_cfg) {
        Serial.println("No modules found");
        while(true) {
            // nop
        }
    }

    for (int i = 0; ; i++) {
        toml_table_t * module = toml_table_at(modules_cfg, i);
        if (!module) break;

        toml_table_t * mod_cfg = nullptr;
        toml_datum_t type = toml_string_in(module, "module");
        if (!type.ok) {
            sprintf(errBuf, "Module %i has no type.", i);
            Serial.println(errBuf);
            continue;
        }

        toml_datum_t name = toml_string_in(module, "name");
        if (!name.ok) {
            sprintf(errBuf, "Module %i of type '%s' has no name/config.", i, type.u.s);
            Serial.println(errBuf);
        } else {
            mod_cfg = toml_table_in(cfg, name.u.s);
        }

        ViewIF * view = viewBuilder(type.u.s, mod_cfg);
        if (view) {
            m_ring.pushBack(view);
        }

        free(type.u.s);
        free(name.u.s);
    }
}


ViewIF * ViewManager::viewBuilder(char * viewType, toml_table_t * cfg) {
    char errBuf[ERRBUFSIZE];

    ViewIF * newView = nullptr;
    if (strcmp("comics", viewType)) {
        if (!cfg) {
            Serial.println("Comics module needs a config");
        }
        Comics * comics = new Comics();
        comics->setup(*m_display, cfg);
        newView = (ViewIF *) comics;
    } else {
        sprintf(errBuf, "Unknown module type '%s'.", viewType);
        Serial.println(errBuf);
    }

    return newView;
}
