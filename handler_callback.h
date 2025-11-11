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

/**
 * A class responsible for handling callbacks for stanza objects in the libstrophe_cpp library.
 * Encapsulates a stanza handler function and its associated context.
 */
class handler_callback {
private:
    StanzaHandler handler;
    libstrophe_cpp *that;

public:
    /**
     * Constructs a handler_callback object with the given stanza handler function and context.
     *
     * @param h A function pointer to the handler of type StanzaHandler, responsible for processing stanza objects.
     * @param that A pointer to the libstrophe_cpp instance associated with this handler.
     */
    explicit handler_callback(StanzaHandler h, libstrophe_cpp *that) : handler(h), that(that) {
    }

    /**
     * Executes the stanza handler callback function with the provided stanza object.
     *
     * @param stanza A pointer to the xmpp_stanza object representing the stanza to be processed.
     */
    void call(xmpp_stanza *stanza) const {
        handler(that, stanza);
        std::cout << "called\n";
    }
};

#endif //LIBSTROPHE_CPP_TEST_HANDLER_CALLBACK_H
