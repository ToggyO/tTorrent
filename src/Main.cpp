#include <iostream>
#include <string>

#include "lib/bencode/bencode.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " bencode <encoded_value>" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "bencode") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " bencode <encoded_value>" << std::endl;
            return 1;
        }
        // You can use print statements as follows for debugging, they'll be visible when running tests.
//        std::cout << "Logs from your program will appear here!" << std::endl; TODO: remove

        // Uncomment this block to pass the first stage
         std::string encoded_value = argv[2];
         json decoded_value = bencode::decode(encoded_value);
         std::cout << decoded_value.dump() << std::endl;
    } else {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}