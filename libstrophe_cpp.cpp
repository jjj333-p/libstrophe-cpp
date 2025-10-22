//
// Created by joseph on 9/28/25.
//

#include "libstrophe_cpp.h"

#include <iostream>

#include "strophe.h"
#include "xmpp_stanza.h"

libstrophe_cpp::libstrophe_cpp(const xmpp_log_level_t log_level, const std::string &jid, const std::string &pass) {
    // Initialize the XMPP library
    xmpp_initialize();

    // Set up logging
    log = xmpp_get_default_logger(log_level);

    // create a new xmpp context
    ctx = xmpp_ctx_new(nullptr, log);

    // create a new xmpp connection
    conn = xmpp_conn_new(ctx);

    // Set connection credentials
    xmpp_conn_set_jid(conn, jid.c_str()); // Set Jabber ID
    xmpp_conn_set_pass(conn, pass.c_str()); // Set password
}

// Clean up resources
libstrophe_cpp::~libstrophe_cpp() {
    xmpp_conn_release(conn);
    xmpp_ctx_free(ctx);
    xmpp_shutdown();
    delete log;
}

void libstrophe_cpp::connect() {
    xmpp_connect_client(conn, nullptr, 0, conn_handler, this);
    xmpp_run(ctx);
    return;
}


void libstrophe_cpp::conn_handler(xmpp_conn_t *conn, xmpp_conn_event_t status, int error,
                                  xmpp_stream_error_t *stream_error, void *userdata) {
    // Suppress unused parameter warnings
    (void) error;
    (void) stream_error;

    auto that = static_cast<libstrophe_cpp *>(userdata);

    if (status == XMPP_CONN_CONNECT) {
        std::cerr << "Connected." << std::endl;

        // Register the message handler for incoming messages
        xmpp_handler_add(conn, generic_handler, nullptr, nullptr, nullptr, that);

        // Send initial presence to indicate the bot is online
        xmpp_stanza_t *pres = xmpp_presence_new(that->ctx);
        xmpp_send(conn, pres);
        xmpp_stanza_release(pres);
    } else {
        // Handle disconnection
        std::cerr << "Disconnected." << std::endl;
        xmpp_stop(that->ctx); // Stop the XMPP context
    }
}

void libstrophe_cpp::set_handler(std::string ns, std::string name, std::string type,
                                 void (*handler)(libstrophe_cpp *client, xmpp_stanza_t *stanza)) {
    //very jank pseudo-hash, maybe wrap it in a class later
    std::string key = std::format("{}/{}/{}", ns, name, type);
    handlers[key] = handler;
}


int libstrophe_cpp::generic_handler(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *_userdata) {
    /*
     *this function has to be static to be a c style function for libstrophe to accept it.
     *libstrophe gives it back to us as a void pointer which we have to cast back.
     *`this` is a reserved keyword so `that` is a play with words.
     */
    auto that = static_cast<libstrophe_cpp *>(_userdata);

    //not sure why this would ever happen but hey
    if (!stanza) {
        return 1;
    }

    //put the libstrophe stanza object in our RAII class
    auto s = xmpp_stanza(stanza);

    //form our key hash used in set_handler
    std::string ns = s.get_attribute("xmlns");
    std::string name = s.get_attribute("name");
    std::string type = s.get_attribute("type");
    std::string key = std::format("{}/{}/{}", ns, name, type);

    //try to find the out of library handler set for that hash
    auto handler = that->handlers.find(key);

    //debug, TODO use proper logging
    if (handler == that->handlers.end()) {
        std::string stringified = s.to_string(that->ctx);
        std::cerr << "No handler for " << key << " || " << stringified << std::endl;
        return 1;
    }

    //call the handler
    handler->second(that, stanza);

    return 1;
}


