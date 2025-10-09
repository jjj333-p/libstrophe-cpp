//
// Created by joseph on 9/28/25.
//

#include "libstrophe_cpp.h"

#include <cstring>
#include <iostream>

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
        xmpp_handler_add(conn, message_handler, nullptr, "message", nullptr, that);

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

int libstrophe_cpp::message_handler(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *_userdata) {
    auto that = static_cast<libstrophe_cpp *>(_userdata);

    // Extract the message body from the incoming stanza
    xmpp_stanza_t *body = xmpp_stanza_get_child_by_name(stanza, "body");
    if (!body) return 1; // If no body found, ignore the message

    // Check if the message is an error message
    const char *type = xmpp_stanza_get_type(stanza);
    if (type && std::strcmp(type, "error") == 0) return 1; // Ignore error messages

    // Get the text content of the message, convert to modern string
    char *intext_char = xmpp_stanza_get_text(body);
    std::string intext = intext_char;
    xmpp_free(that->ctx, intext_char);


    // Print the received message and sender information
    std::cout << "!! Message from " << xmpp_stanza_get_from(stanza) << ": " << intext << std::endl;

    // Create a reply stanza based on the incoming message
    xmpp_stanza_t *reply = xmpp_stanza_reply(stanza);

    // Set the message type to "chat" if not already set
    if (!xmpp_stanza_get_type(reply))
        xmpp_stanza_set_type(reply, "chat");

    // Handle the "quit" command or create an echo response
    std::string replytext;
    if (intext == "quit") {
        replytext = "Goodbye!";
        xmpp_disconnect(conn); // Disconnect on quit command
    } else {
        replytext = std::format("You said: {}", intext);
    }

    // Set the reply body and send the message
    xmpp_message_set_body(reply, replytext.c_str());
    xmpp_send(conn, reply);

    // Clean up allocated resources
    xmpp_stanza_release(reply);

    return 1;
}


