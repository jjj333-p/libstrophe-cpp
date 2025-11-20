//
// Created by joseph on 9/28/25.
//

#ifndef LIBSTROPHE_CPP_TEST_LIBSTROPHE_CPP_H
#define LIBSTROPHE_CPP_TEST_LIBSTROPHE_CPP_H
#include <memory>
#include <string>
#include <strophe.h>
#include <unordered_map>

#include "handler_callback.h"
#include "xmpp_stanza.h"

//foward declaration
class xmpp_stanza;

/**
 * Represents a C++ wrapper for the libstrophe library to facilitate XMPP protocol handling.
 *
 * The `libstrophe_cpp` class provides an interface for creating and managing XMPP client connections,
 * handling events, and managing the lifecycle of XMPP streams.
 *
 * This class is designed to simplify interaction with the libstrophe library while ensuring a
 * user-friendly API for incorporating XMPP functionality into C++ applications.
 *
 * Key responsibilities include:
 * - Establishing and managing XMPP connections.
 * - Registering and invoking user-defined handlers for XMPP events.
 * - Abstracting libstrophe internals for ease of use in a modern C++ environment.
 *
 * Refer to the specific methods of this class for further details on the API.
 */
class libstrophe_cpp {
private:
    // internal constants of the xmpp connection
    const xmpp_log_t *log;
    xmpp_conn_t *conn;

    //internal handler for things
    /**
     * Handles XMPP connection events such as connection, disconnection, and errors.
     *
     * @param conn Pointer to the XMPP connection object.
     * @param status The connection event status, e.g., connected, disconnected, or connection error.
     * @param error The error code, if any, during the connection event.
     * @param stream_error Pointer to additional stream error information, if present.
     * @param userdata User-defined data passed to the handler, typically the application context or object.
     */
    static void conn_handler(xmpp_conn_t *conn, xmpp_conn_event_t status, int error,
                             xmpp_stream_error_t *stream_error, void *userdata);

    //internal map of handlers
    std::unordered_map<std::string, std::unique_ptr<handler_callback> > handlers;

    //type for finding stanzas
    struct HandlerStrings {
        std::string ns, name, type;
    };

    std::unordered_map<std::string, HandlerStrings> handler_strings;

public:
    xmpp_ctx_t *ctx;

    //allocate the libstrophe stuff
    ///
    libstrophe_cpp(xmpp_log_level_t log_level, const std::string &jid, const std::string &pass);

    //libstrophe specific deallocation
    ~libstrophe_cpp();

    /**
     * Initiates an XMPP connection using the configured connection instance and runs the associated context.
     *
     * This method establishes a client connection to the XMPP server using the pre-configured connection object
     * and connection handler. Once the connection is established or attempted, the method executes the main
     * event loop for processing XMPP stanzas and events.
     */
    void connect();

    /**
     * Registers a handler for processing XMPP stanzas based on specified criteria.
     *
     * This method allows the user to define a handler function that will be called
     * when an XMPP stanza matching the specified namespace, name, and type is received.
     * The handler function operates on the provided libstrophe_cpp instance and stanza.
     *
     * @param ns The namespace of the XMPP stanza to match.
     * @param name The name of the XMPP stanza to match.
     * @param type The type of the XMPP stanza to match.
     * @param handler A pointer to the function to handle the XMPP stanza when received.
     */
    void set_handler(std::string ns, std::string name, std::string type,
                     void (*handler)(libstrophe_cpp *client, xmpp_stanza *stanza));

    void send(xmpp_stanza *stanza);

    /**
     * Deallocates memory that was dynamically allocated for XMPP-related operations. IDK about it tbh.
     *
     * This method wraps the libstrophe `xmpp_free` function to free memory that
     * was allocated within the XMPP context. It ensures proper memory management
     * by leveraging the context associated with the libstrophe instance.
     *
     * @param thing Pointer to the memory region to be deallocated. This should be
     * dynamically allocated memory that is valid within the lifetime of the
     * XMPP context associated with the `libstrophe_cpp` instance.
     */
    void free(void *thing) const {
        xmpp_free(ctx, thing);
    }
};


#endif //LIBSTROPHE_CPP_TEST_LIBSTROPHE_CPP_H
