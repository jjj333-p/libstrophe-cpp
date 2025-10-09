//
// Created by joseph on 10/7/25.
//

#ifndef LIBSTROPHE_CPP_TEST_XMPP_MESSAGE_H
#define LIBSTROPHE_CPP_TEST_XMPP_MESSAGE_H
#include <string>
#include <strophe.h>
#include <unordered_map>

#include "libstrophe_cpp.h"


class xmpp_message {
private:
    xmpp_stanza_t *stanza;
    std::unordered_map<std::string, std::string> attributes;

public:
    xmpp_message(libstrophe_cpp *client, xmpp_stanza_t *this_stanza);
};


#endif //LIBSTROPHE_CPP_TEST_XMPP_MESSAGE_H
