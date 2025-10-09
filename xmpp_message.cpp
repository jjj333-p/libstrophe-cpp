//
// Created by joseph on 10/7/25.
//

#include "xmpp_message.h"

xmpp_message::xmpp_message(libstrophe_cpp *client, xmpp_stanza_t *s) : stanza(s) {
    int n_atributes = xmpp_stanza_get_attribute_count(stanza);

    auto _libstrophe_atribute_list = new const char *[n_atributes * 2];

    xmpp_stanza_get_attributes(stanza, _libstrophe_atribute_list, n_atributes);

    for (int i = 0; i < n_atributes * 2; i += 2) {
        auto k_chars = _libstrophe_atribute_list[i];
        auto v_chars = _libstrophe_atribute_list[i + 1];

        std::string k(k_chars);
        std::string v(v_chars);

        attributes[k] = v;
    }
}
