//
// Created by joseph on 10/21/25.
//

#ifndef LIBSTROPHE_CPP_TEST_HANDLER_CALLBACK_H
#define LIBSTROPHE_CPP_TEST_HANDLER_CALLBACK_H
#include <functional>
#include <iostream>
#include <utility>

#include "xmpp_stanza.h"

// Forward declaration
class libstrophe_cpp;
class xmpp_stanza;

using StanzaHandler = void (*)(libstrophe_cpp *that, xmpp_stanza *);

class handler_callback {
private:
    StanzaHandler handler;
    libstrophe_cpp *that;

public:
    explicit handler_callback(StanzaHandler h, libstrophe_cpp *that) : handler(h), that(that) {
    }

    void call(xmpp_stanza *stanza) const {
        handler(that, stanza);
        std::cout << "called\n";
    }
};

#endif //LIBSTROPHE_CPP_TEST_HANDLER_CALLBACK_H
