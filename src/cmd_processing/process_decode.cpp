#include "processors.h"
#include "../lib/json/json_bencode.h"

int process_decode(int argc, char* argv[])
{
    // TODO: try...catch
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
        return 1;
    }

    std::string encoded_value = argv[2];
    auto decoded_value = bencode::decode(encoded_value);
    std::cout << ((json)decoded_value).dump() << std::endl;
    return 0;
}
