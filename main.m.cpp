#include <smithers.h>
#include <m2pp.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

int main(int argc, char *argv[]) {
    std::string sender_id = "82209006-86FF-4982-B5EA-D1E29E55D481";

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <from> <to>\n"
            "\tExample: " << argv[0] << " 'tcp://127.0.0.1:9999'"
            " 'tcp://127.0.0.1:9998'" << std::endl;
        return 1;
    }

    smithers::Smithers x;
    x.await_registered_players(2);

    std::string y;
    std::cout << "..continue?" << std::endl;
    std::cin >> y;

    x.print_players();

    m2pp::connection conn(sender_id, argv[1], argv[2]);

    return 0;
}
