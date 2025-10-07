

#include <fstream>
#include <iostream>

#include "libstrophe_cpp.h"

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

    lsc.connect();

    return 0;
}
