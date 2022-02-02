//
// Created by fraggy on 25/12/2021.
//

#ifndef INKPLATE_COMICS_VIEWRING_H
#define INKPLATE_COMICS_VIEWRING_H

#include "ViewIF.h"

class ViewRing {
public:
    ViewRing();
    virtual ~ViewRing();

    void drain();

    void pushBack(ViewIF * item);
    ViewIF * getCurrent();
    ViewIF * getNext();

private:
    class Item {
    public:
        Item(ViewIF * content);
        Item(ViewIF * content, Item * next);
        virtual ~Item();

        Item * m_next;
        ViewIF * m_content;
    };

    Item * getLast();

    Item * m_head;
    Item * m_curr;

};


#endif //INKPLATE_COMICS_VIEWRING_H
