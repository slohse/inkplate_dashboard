//
// Created by fraggy on 25/12/2021.
//

#include <cstring>
#include "ViewManager.h"
#include "Comics.h"
#include "Text.h"
#include "../shared_consts.h"

#define VERBOSE

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
    char errBuf[ERRBUFSIZE];

    ViewIF * curr = m_ring.getNext();
    if (!curr) {
        Serial.println("ViewManager::centerButton> no current view");
        return;
    }
    sprintf(errBuf, "Resuming %p.", (void *) curr);
    Serial.println(errBuf);
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
#ifdef VERBOSE
        sprintf(errBuf, "Looking at module at index %i.", i);
        Serial.println(errBuf);
#endif
        toml_table_t * module = toml_table_at(modules_cfg, i);
        if (!module) break;

        ViewIF * view = viewBuilder(module);
#ifdef VERBOSE
        sprintf(errBuf, "New view at %p.", (void *) view);
        Serial.println(errBuf);
#endif
        if (view) {
#ifdef VERBOSE
            sprintf(errBuf, "Adding view %p to the ring.", (void *) view);
            Serial.println(errBuf);
#endif
            m_ring.pushBack(view);
        }
    }
}


ViewIF * ViewManager::viewBuilder(toml_table_t * mod_cfg) {
    char errBuf[ERRBUFSIZE];

    toml_datum_t type = toml_string_in(mod_cfg, "module");
    if (!type.ok) {
        Serial.println("Module has no type.");
        return nullptr;
    }

#ifdef VERBOSE
    sprintf(errBuf, "Module type is %s.", type.u.s);
    Serial.println(errBuf);
#endif

    ViewIF * newView = nullptr;
    if (strcmp("comics", type.u.s) == 0) {
        Comics * comics = new Comics();
        if (comics->setup(*m_display, mod_cfg)) {
            newView = (ViewIF *) comics;
        } else {
            sprintf(errBuf, "Could not set up comics module.");
        }
    } else if (strcmp("text", type.u.s) == 0) {
        Text * text = new Text();
        if (text->setup(*m_display, mod_cfg)) {
            newView = (ViewIF *) text;
        } else {
            sprintf(errBuf, "Could not set up text module.");
        }
    } else {
        sprintf(errBuf, "Unknown module type '%s'.", type.u.s);
        Serial.println(errBuf);
    }

    free(type.u.s);

    return newView;
}
