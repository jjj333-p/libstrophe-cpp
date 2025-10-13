//
// Created by joseph on 10/7/25.
//

#ifndef LIBSTROPHE_CPP_TEST_XMPP_MESSAGE_H
#define LIBSTROPHE_CPP_TEST_XMPP_MESSAGE_H
#include <string>
#include <strophe.h>
#include <unordered_map>
#include <memory>
#include <optional>
#include <vector>

#include "libstrophe_cpp.h"

class xmpp_stanza {
private:
    xmpp_stanza_t *stanza;
    std::unordered_map<std::string, std::string> attributes;

public:
    explicit xmpp_stanza(xmpp_stanza_t *s);

    ~xmpp_stanza();

    // Move constructor
    xmpp_stanza(xmpp_stanza &&other) noexcept;

    // Move assignment
    xmpp_stanza &operator=(xmpp_stanza &&other) noexcept;

    // Accessor methods
    const std::unordered_map<std::string, std::string> &get_attributes() const { return attributes; }

    std::string get_attribute(const std::string &key) const;

    std::optional<xmpp_stanza> get_child_element(const std::string &name, const std::string &xmlns) const;

    std::string to_string(const xmpp_ctx_t *ctx) const;
};

#endif //LIBSTROPHE_CPP_TEST_XMPP_MESSAGE_H
