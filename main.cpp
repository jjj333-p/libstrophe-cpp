#include <fstream>
#include <iostream>
#include <thread>
#include <random>

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
        {"to", from},
    };
    std::unordered_map<std::string, std::string> body_content = {{"body", "Echo: \"" + message + "\""}};
    auto newmsg = xmpp_stanza(stanza, &attrs, &body_content, nullptr);

    client->send(&newmsg);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 2);
    int random_value = distrib(gen);

    std::uniform_int_distribution<> distrib_100(1, 100);
    int random_value_100 = distrib_100(gen);


    std::string bdy;
    switch (random_value) {
        case 0:
            bdy = "Battery: " + std::to_string(random_value_100) + "%";
            break;
        case 1:
            bdy = "Signal: " + std::to_string(random_value_100) + "%";
            break;
        case 2:
            bdy = "Speed: " + std::to_string(random_value_100) + " km/h";
    }

    std::unordered_map<std::string, std::string> second_body = {{"body", bdy}};
    auto second_msg = xmpp_stanza(stanza, &attrs, &second_body, nullptr);

    client->send(&second_msg);
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

    int connstate = 0;
    do {
        connstate = lsc.connect_noexcept();
        std::cout << "exited";
    } while (connstate != 0);

    return 0;
}
