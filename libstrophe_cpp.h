//
// Created by joseph on 9/28/25.
//

#ifndef LIBSTROPHE_CPP_TEST_LIBSTROPHE_CPP_H
#define LIBSTROPHE_CPP_TEST_LIBSTROPHE_CPP_H
#include <string>
#include <strophe.h>


class libstrophe_cpp {
private:
    // internal constants of the xmpp connection
    const xmpp_log_t *log;
    xmpp_ctx_t *ctx;
    xmpp_conn_t *conn;

    //internal handler for things
    void conn_handler(xmpp_conn_t *conn, xmpp_conn_event_t status, int error,
                      xmpp_stream_error_t *stream_error, void *userdata);

    //internal message handler callback
    static int message_handler(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *_userdata);

public:
    //allocate the libstrophe stuff
    libstrophe_cpp(xmpp_log_level_t log_level, const std::string &jid, const std::string &pass);

    //libstrophe specific deallocation
    ~libstrophe_cpp();
};


#endif //LIBSTROPHE_CPP_TEST_LIBSTROPHE_CPP_H
