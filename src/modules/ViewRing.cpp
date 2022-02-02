//
// Created by fraggy on 25/12/2021.
//

#include "ViewRing.h"


ViewRing::ViewRing() {
    m_head = nullptr;
    m_curr = nullptr;
}

ViewRing::~ViewRing() {
    drain();
}

void ViewRing::drain() {
    if (m_head) {
        Item * curr = m_head;
        while(curr) {
            Item * next = curr->m_next;
            delete curr->m_content;
            delete curr;
            curr = next;
        }
    }
}

void ViewRing::pushBack(ViewIF * item) {
    Item * new_view = new Item(item);
    Item * last = getLast();
    if (last) {
        last->m_next = new_view;
    } else {
        m_head = new_view;
        m_curr = m_head;
    }
}

ViewIF * ViewRing::getCurrent() {
    return m_curr->m_content;
}

ViewIF * ViewRing::getNext() {
    if (m_curr && m_curr->m_next) {
        m_curr = m_curr->m_next;
    } else {
        m_curr = m_head;
    }

    if (m_curr) {
        return m_curr->m_content;
    }

    return nullptr;
}


// private

ViewRing::Item::Item(ViewIF * content) {
    m_content = content;
    m_next = nullptr;
}

ViewRing::Item::Item(ViewIF * content, Item * next) {
    m_content = content;
    m_next = next;
}

ViewRing::Item::~Item(){

}

ViewRing::Item * ViewRing::getLast() {
    if (m_head) {
        Item * curr = m_head;
        while(curr->m_next) {
            curr = curr->m_next;
        }

        return curr;
    }

    return nullptr;
}
