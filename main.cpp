#include <fstream>
#include <iostream>

#include "libstrophe_cpp.h"

void h(libstrophe_cpp *client, xmpp_stanza *stanza) {
    std::cout << "in h" << std::endl;
    auto body = stanza->get_child_element_text("body", "jabber:client");

    const std::string message = body ? *body : "no body";
    std::cout << "Got a message: " << message << std::endl;

    if (!stanza->is_valid()) {
        return;
    }

    // auto type = stanza->get_attribute("type");
    // if (type != "chat") return;

    auto from = stanza->get_attribute("from");
    std::unordered_map<std::string, std::string> attrs = {
        {"type", "chat"},
        {"to", "jjj333@pain.agency"},
    };
    std::unordered_map<std::string, std::string> body_content = {{"body", "Recieved \"" + message + "\""}};
    auto newmsg = xmpp_stanza(stanza, &attrs, &body_content, nullptr);

    client->send(&newmsg);
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

    libstrophe_cpp lsc(XMPP_LEVEL_DEBUG, jid, password);

    std::cout << "input" << &h << std::endl;
    lsc.set_handler(std::nullopt, "message", "chat", h);

    lsc.connect();

    return 0;
}
