//
// Created by joseph on 10/7/25.
//

#ifndef LIBSTROPHE_CPP_TEST_XMPP_MESSAGE_H
#define LIBSTROPHE_CPP_TEST_XMPP_MESSAGE_H
#include <string>
#include <strophe.h>
#include <unordered_map>
#include <memory>

#include "libstrophe_cpp.h"

class xmpp_message {
private:
    xmpp_stanza_t *stanza;
    std::unordered_map<std::string, std::string> attributes;

public:
    xmpp_message(libstrophe_cpp *client, xmpp_stanza_t *this_stanza);

    // Accessor methods
    const std::unordered_map<std::string, std::string> &get_attributes() const { return attributes; }

    std::string get_attribute(const std::string &key) const;
};

#endif //LIBSTROPHE_CPP_TEST_XMPP_MESSAGE_H
