//
// Created by joseph on 10/7/25.
//

#ifndef LIBSTROPHE_CPP_TEST_XMPP_MESSAGE_H
#define LIBSTROPHE_CPP_TEST_XMPP_MESSAGE_H
#include <string>
#include <strophe.h>
#include <unordered_map>
#include <optional>


/**
 * Provides a wrapper around a libstrophe stanza to enhance usability.
 * @param s The xmpp stanza object processed by the libstrophe library.
 */
class xmpp_stanza {
private:
    std::unordered_map<std::string, std::string> attributes;

public:
    //you probably shouldnt touch this unless you know what youre doing
    xmpp_stanza_t *stanza;

    /**
     * construct a friendly class around a libstrophe stanza for conveience
     * @param s xmpp stanza as recieved by the libstrophe library
     */
    explicit xmpp_stanza(xmpp_stanza_t *s);

    /**
     * create stanza to send where a previous stanza came from
     * @param additional_attributes the atributes section of the xml stanza, key,value pairs
     * @param string_elements children of the xml stanza that are just string content, i.e. <body>foo</body>, key,value pairs
     * @param children children stanzas of the xml stanza, key,xmpp_stanza pairs
     */
    xmpp_stanza(
        xmpp_stanza *original,
        std::unordered_map<std::string, std::string> *additional_attributes,
        std::unordered_map<std::string, std::string> *string_elements,
        std::unordered_map<std::string, xmpp_stanza> *children
    );

    /**
     * Create a fresh new stanza object
     * @param ctx libstrophe context, might not be important, will not be saved
     * @param atributes the atributes section of the xml stanza, key,value pairs
     * @param string_elements children of the xml stanza that are just string content, i.e. <body>foo</body>, key,value pairs
     * @param children children stanzas of the xml stanza, key,xmpp_stanza pairs
     */
    xmpp_stanza(
        xmpp_ctx_t *ctx,
        std::unordered_map<std::string, std::string> *atributes,
        std::unordered_map<std::string, std::string> *string_elements,
        std::unordered_map<std::string, xmpp_stanza> *children
    );

    //Destroy
    ~xmpp_stanza();

    // Move constructor
    xmpp_stanza(xmpp_stanza &&other) noexcept;

    // Move assignment
    xmpp_stanza &operator=(xmpp_stanza &&other) noexcept;

    /**
     * Retrieves the value of the specified attribute in this stanza, the "key"="value" part.
     * @param key The name of the attribute to retrieve.
     * @return The value of the attribute if found, or an empty string if the attribute is not present.
     */
    std::string get_attribute(const std::string &key) const;

    /**
     * Sets an attribute key-value pair for the stanza.
     * @param key The name of the attribute to be set.
     * @param value The value to assign to the attribute.
     */
    void set_attribute(const std::string &key, const std::string &value);

    /**
     * Retrieves a child element of the current stanza that matches the specified name and XML namespace.
     * @param name The name of the child element to search for.
     * @param xmlns The XML namespace of the child element to search for.
     * @return An optional xmpp_stanza object representing the child element if found, or std::nullopt if no matching child exists.
     */
    std::optional<xmpp_stanza> get_child_element(const std::string &name, const std::string &xmlns) const;


    /**
     * Sets a child element for the current stanza using a given name and xmpp_stanza object.
     * @param name The name of the child element to be set.
     * @param child The xmpp_stanza object representing the child element to attach.
     */
    void set_child_element(const std::string &name, xmpp_stanza &child) const;

    /**
     * Retrieves the text content of a child element in the current stanza that matches the specified name and XML namespace.
     * @param name The name of the child element whose text content is to be retrieved.
     * @param xmlns The XML namespace of the child element to search for.
     * @return An optional string containing the text content of the matching child element if it exists, or std::nullopt if no matching child element is found.
     */
    std::optional<std::string> get_child_element_text(const std::string &name, const std::string &xmlns) const;

    /**
     * Sets a child element for the current stanza using a given name and text content, such as the body.
     * @param name The name of the child element to be set.
     * @param text The text content to associate with the child element.
     */
    void set_child_element(const std::string &name, const std::string &text) const;

    /**
     * Converts the XMPP stanza to its textual representation.
     * @return A string containing the textual representation of the XMPP stanza.
     * If the stanza does not contain text, an empty string is returned.
     */
    std::string to_string() const;

    /**
     * Checks if the xmpp_stanza object is valid.
     * @return True if the underlying stanza object is not null, otherwise false.
     */
    bool is_valid() const {
        return stanza != nullptr;
    }
};

#endif //LIBSTROPHE_CPP_TEST_XMPP_MESSAGE_H
