//
// Created by joseph on 9/28/25.
//

#include "libstrophe_cpp.h"
#include "strophe.h"

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

void libstrophe_cpp::conn_handler(xmpp_conn_t *conn, xmpp_conn_event_t status, int error,
                                  xmpp_stream_error_t *stream_error, void *userdata) {
    // Suppress unused parameter warnings
    (void) error;
    (void) stream_error;

    if (status == XMPP_CONN_CONNECT) {
        std::cerr << "Connected." << std::endl;

        // Register the message handler for incoming messages
        xmpp_handler_add(conn, message_handler, nullptr, "message", nullptr, ctx);

        // Send initial presence to indicate the bot is online
        xmpp_stanza_t *pres = xmpp_presence_new(ctx);
        xmpp_send(conn, pres);
        xmpp_stanza_release(pres);
    } else {
        // Handle disconnection
        std::cerr << "Disconnected." << std::endl;
        xmpp_stop(ctx); // Stop the XMPP context
    }
}

