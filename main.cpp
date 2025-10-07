#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <string>
#include <format>

// Include the C-based libstrophe XMPP library
extern "C" {
#include <strophe.h>
}

// Message handler callback function
// Processes incoming XMPP messages and sends appropriate responses
int message_handler(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *userdata) {
    // Cast the userdata pointer to the XMPP context
    const auto *ctx = static_cast<xmpp_ctx_t *>(userdata);

    // Extract the message body from the incoming stanza
    xmpp_stanza_t *body = xmpp_stanza_get_child_by_name(stanza, "body");
    if (!body) return 1; // If no body found, ignore the message

    // Check if the message is an error message
    const char *type = xmpp_stanza_get_type(stanza);
    if (type && std::strcmp(type, "error") == 0) return 1; // Ignore error messages

    // Get the text content of the message, convert to modern string
    char *intext_char = xmpp_stanza_get_text(body);
    std::string intext = intext_char;
    xmpp_free(ctx, intext_char);


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

    return 1; // Return 1 to keep the handler active
}

// Connection event handler
// Manages connection state changes and initializes message handling
void conn_handler(xmpp_conn_t *conn, xmpp_conn_event_t status, int error,
                  xmpp_stream_error_t *stream_error, void *userdata) {
    auto *ctx = static_cast<xmpp_ctx_t *>(userdata);
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

int main() {
    std::ifstream file("../db/login.txt");
    if (!file.is_open()) {
        std::cerr << "Could not open login.txt" << std::endl;
        return 1;
    }

    std::string jid, password;
    std::getline(file, jid);
    std::getline(file, password);
    file.close();


    // Initialize the XMPP library
    xmpp_initialize();

    // Set up logging
    const xmpp_log_t *log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);

    // Create a new XMPP context
    xmpp_ctx_t *ctx = xmpp_ctx_new(nullptr, log);

    // Create a new XMPP connection
    xmpp_conn_t *conn = xmpp_conn_new(ctx);

    // Set connection credentials
    xmpp_conn_set_jid(conn, jid.c_str()); // Set Jabber ID
    xmpp_conn_set_pass(conn, password.c_str()); // Set password

    // Initialize the client connection
    xmpp_connect_client(conn, nullptr, 0, conn_handler, ctx);

    // Start the main event loop
    xmpp_run(ctx);

    // Clean up resources
    xmpp_conn_release(conn);
    xmpp_ctx_free(ctx);
    xmpp_shutdown();
    return 0;
}
