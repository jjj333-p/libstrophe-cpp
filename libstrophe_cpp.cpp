//
// Created by joseph on 9/28/25.
//

#include "libstrophe_cpp.h"

#include <functional>
#include <iostream>

#include "handler_callback.h"
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
}

int c_callback_for_libstrophe(xmpp_conn_t *, xmpp_stanza_t *stanza, void *_userdata) {
    auto handler = static_cast<handler_callback *>(_userdata);
    auto s = xmpp_stanza(stanza);
    handler->call(&s);
    return 1;
};

void libstrophe_cpp::conn_handler(xmpp_conn_t *conn, xmpp_conn_event_t status, int error,
                                  xmpp_stream_error_t *stream_error, void *userdata) {
    // Suppress unused parameter warnings
    // TODO: handle warnings
    (void) error;
    (void) stream_error;

    auto that = static_cast<libstrophe_cpp *>(userdata);

    if (status == XMPP_CONN_CONNECT) {
        std::cerr << "Connected." << std::endl;

        for (const auto &hdlr: that->handler_strings) {
            std::string key = hdlr.first;
            HandlerStrings strings = hdlr.second;

            xmpp_handler_add(
                conn, //libstrophe connection object
                c_callback_for_libstrophe, //generic handler lambda function defined above
                //TODO why does this need to be nullptr
                nullptr, strings.name.c_str(), nullptr, //filters
                that->handlers[key].get() //object map containing the handler callbacks
            );
        }

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
                                 void (*handler)(libstrophe_cpp *client, xmpp_stanza *stanza)) {
    //very jank pseudo-hash, maybe wrap it in a class later
    std::string key = std::format("{}/{}/{}", ns, name, type);
    handlers[key] = std::make_unique<handler_callback>(handler_callback(handler, this)); //handler;

    // Store the strings to keep them alive
    handler_strings[key] = {ns, name, type};
}

void libstrophe_cpp::send(const xmpp_stanza *stanza) const {
    if (conn && stanza != nullptr && stanza->is_valid()) {
        xmpp_send(conn, stanza->stanza);
    }
}




