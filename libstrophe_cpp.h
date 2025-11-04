//
// Created by joseph on 9/28/25.
//

#ifndef LIBSTROPHE_CPP_TEST_LIBSTROPHE_CPP_H
#define LIBSTROPHE_CPP_TEST_LIBSTROPHE_CPP_H
#include <string>
#include <strophe.h>
#include <unordered_map>

#include "handler_callback.h"
#include "xmpp_stanza.h"

//foward declaration
class xmpp_stanza;

class libstrophe_cpp {
private:
    // internal constants of the xmpp connection
    const xmpp_log_t *log;
    xmpp_conn_t *conn;

    //internal handler for things
    static void conn_handler(xmpp_conn_t *conn, xmpp_conn_event_t status, int error,
                             xmpp_stream_error_t *stream_error, void *userdata);

    //internal map of handlers
    std::unordered_map<std::string, std::unique_ptr<handler_callback> > handlers;

    struct HandlerStrings {
        std::string ns, name, type;
    };

    std::unordered_map<std::string, HandlerStrings> handler_strings;

public:
    xmpp_ctx_t *ctx;

    //allocate the libstrophe stuff
    libstrophe_cpp(xmpp_log_level_t log_level, const std::string &jid, const std::string &pass);

    //libstrophe specific deallocation
    ~libstrophe_cpp();

    void connect();

    void set_handler(std::string ns, std::string name, std::string type,
                     void (*handler)(libstrophe_cpp *client, xmpp_stanza *stanza));

    void free(void *thing) const {
        xmpp_free(ctx, thing);
    }

};


#endif //LIBSTROPHE_CPP_TEST_LIBSTROPHE_CPP_H
