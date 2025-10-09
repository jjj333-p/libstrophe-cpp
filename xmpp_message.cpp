//
// Created by joseph on 10/7/25.
//

#include "xmpp_message.h"

#include <memory>
#include <stdexcept>

xmpp_message::xmpp_message(libstrophe_cpp *client, xmpp_stanza_t *s) : stanza(s) {
    if (!client) {
        throw std::invalid_argument("Client cannot be null");
    }

    if (!stanza) {
        throw std::invalid_argument("Stanza cannot be null");
    }


    int n_attributes = xmpp_stanza_get_attribute_count(stanza);

    if (n_attributes <= 0) {
        return; // No attributes to process
    }

    auto _libstrophe_attribute_list = std::make_unique<const char *[]>(n_attributes * 2);

    xmpp_stanza_get_attributes(stanza, _libstrophe_attribute_list.get(), n_attributes);

    for (int i = 0; i < n_attributes * 2; i += 2) {
        auto k_chars = _libstrophe_attribute_list[i];
        auto v_chars = _libstrophe_attribute_list[i + 1];

        // Add null checks for safety
        if (k_chars && v_chars) {
            attributes[std::string(k_chars)] = std::string(v_chars);
        }
    }
}

std::string xmpp_message::get_attribute(const std::string &key) const {
    const auto it = attributes.find(key);
    //if no item
    if (it == attributes.end()) {
        return "";
    }

    //return item
    return it->second;
}