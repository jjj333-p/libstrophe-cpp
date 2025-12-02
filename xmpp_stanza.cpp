//
// Created by joseph on 10/7/25.
//

#include "xmpp_stanza.h"

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>

// If your strings might contain the delimiter, escape it
std::string make_key(const std::string &tag, const std::string &attr) {
    return tag + "\x1F" + attr; // Using ASCII Unit Separator character
}

xmpp_stanza::xmpp_stanza(const xmpp_stanza_t *s) {
    // make sure we got a stanza and store a local copy
    if (!s) {
        throw std::invalid_argument("Stanza cannot be null");
    }
    stanza = xmpp_stanza_copy(s);

    int n_attributes = xmpp_stanza_get_attribute_count(stanza);

    if (n_attributes <= 0) {
        return; // No attributes to process
    }

    auto _libstrophe_attribute_list = std::make_unique<const char *[]>(n_attributes * 2);

    xmpp_stanza_get_attributes(stanza, _libstrophe_attribute_list.get(), n_attributes * 2);

    for (int i = 0; i < n_attributes * 2; i += 2) {
        auto k_chars = _libstrophe_attribute_list[i];
        auto v_chars = _libstrophe_attribute_list[i + 1];

        // Add null checks for safety
        if (k_chars && v_chars) {
            std::string key = k_chars;
            std::string value = v_chars;
            attributes[key] = value;
        }
    }
}

xmpp_stanza::xmpp_stanza(
    const xmpp_stanza *original,
    std::unordered_map<std::string, std::string> *additional_attributes,
    std::unordered_map<std::string, std::string> *string_elements,
    std::unordered_map<std::string, xmpp_stanza> *children
) {
    //create reply stanza
    stanza = xmpp_stanza_reply(original->stanza);

    //copy new attributes (only if pointer is not null)
    if (additional_attributes) {
        for (auto &kv: *additional_attributes) {
            set_attribute(kv.first, kv.second);
        }
    }

    //copy new elements (only if pointer is not null)
    if (string_elements) {
        for (auto &kv: *string_elements) {
            set_child_element(kv.first, kv.second);
        }
    }

    //copy new children (only if pointer is not null)
    if (children) {
        for (auto &kv: *children) {
            set_child_element(kv.first, kv.second);
        }
    }
}

xmpp_stanza::xmpp_stanza(
    xmpp_ctx_t *ctx,
    std::unordered_map<std::string, std::string> *attributes_map,
    std::unordered_map<std::string, std::string> *string_elements,
    std::unordered_map<std::string, xmpp_stanza> *children
) : stanza(xmpp_stanza_new(ctx)) {
    // Add null checks before dereferencing
    if (attributes_map) {
        for (auto &kv: *attributes_map) {
            set_attribute(kv.first, kv.second);
        }
    }

    if (string_elements) {
        for (auto &kv: *string_elements) {
            set_child_element(kv.first, kv.second);
        }
    }

    if (children) {
        for (auto &kv: *children) {
            set_child_element(kv.first, kv.second);
        }
    }
}

xmpp_stanza::~xmpp_stanza() {
    if (stanza != nullptr) {
        // Add null check before releasing
        xmpp_stanza_release(stanza);
        stanza = nullptr;
    }
}

// Move constructor
xmpp_stanza::xmpp_stanza(xmpp_stanza &&other) noexcept
    : attributes(std::move(other.attributes)), stanza(other.stanza) {
    other.stanza = nullptr; // Transfer ownership
}

// Move assignment operator
xmpp_stanza &xmpp_stanza::operator=(xmpp_stanza &&other) noexcept {
    if (this != &other) {
        // Clean up current resources
        if (stanza) {
            xmpp_stanza_release(stanza); // or whatever cleanup function you use
        }

        // Transfer ownership from other
        stanza = other.stanza;
        attributes = std::move(other.attributes);

        // Leave other in valid but empty state
        other.stanza = nullptr;
    }
    return *this;
}

std::string xmpp_stanza::get_attribute(const std::string &key) const {
    //weird c++ iterator object
    const auto it = attributes.find(key);
    //if no item
    if (it == attributes.end()) {
        return "";
    }

    //return item
    return it->second;
}

void xmpp_stanza::set_attribute(const std::string &key, const std::string &value) {
    xmpp_stanza_set_attribute(stanza, key.c_str(), value.c_str());
    attributes[key] = value;
}

std::optional<xmpp_stanza> xmpp_stanza::get_child_element(
    const std::string &name, const std::string &xmlns) const {
    xmpp_stanza_t *c = xmpp_stanza_get_child_by_name(stanza, name.c_str()); //, xmlns.c_str());
    if (!c) return std::nullopt;
    // If child exists, we need to take a reference to it since libstrophe stanzas are reference counted
    xmpp_stanza_t *child_copy = xmpp_stanza_copy(c);
    return xmpp_stanza(child_copy);
}

void xmpp_stanza::set_child_element(const std::string &name, const xmpp_stanza &child) const {
    xmpp_stanza_t *v = xmpp_stanza_copy(child.stanza);
    xmpp_stanza_set_name(v, name.c_str());
    xmpp_stanza_add_child(stanza, v);
    xmpp_stanza_release(v);
}

std::optional<std::string>
xmpp_stanza::get_child_element_text(const std::string &name, const std::string &xmlns) const {
    auto child = get_child_element(name, xmlns);
    if (!child) return std::nullopt;
    return child->to_string();
}

void xmpp_stanza::set_child_element(const std::string &name, const std::string &text) const {
    xmpp_ctx_t *ctx = xmpp_stanza_get_context(stanza);

    // Create the element (e.g., <body>)
    xmpp_stanza_t *child = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(child, name.c_str());

    // Create a text node child
    xmpp_stanza_t *text_node = xmpp_stanza_new(ctx);
    xmpp_stanza_set_text(text_node, text.c_str());

    // Add text node to the element
    xmpp_stanza_add_child(child, text_node);
    xmpp_stanza_release(text_node);

    // Add the complete element to parent
    xmpp_stanza_add_child(stanza, child);
    xmpp_stanza_release(child);
}


std::string xmpp_stanza::to_string() const {
    char *const chars = xmpp_stanza_get_text(stanza);
    if (!chars) return "";
    std::string s = chars;
    xmpp_free(xmpp_stanza_get_context(stanza), chars); // Use xmpp_free instead of free
    return s;
}

