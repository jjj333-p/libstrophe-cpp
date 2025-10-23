#include <fstream>
#include <iostream>

#include "libstrophe_cpp.h"

void h(libstrophe_cpp *client, xmpp_stanza *stanza) {
    std::cout << "in h" << std::endl;
    std::cout << "Got a message: " << stanza->to_string(client->ctx) << std::endl;
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
    lsc.set_handler("", "message", "chat", h);

    lsc.connect();

    return 0;
}
