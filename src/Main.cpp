#include "commands.h"
#include "cmd_processing/processors.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        // TODO: add usage
        std::cerr << "Usage: " << argv[0] << " [OPTIONS] <command>" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    Command cmd = Command::unknown;
    if (const auto search = command_mapper.find(command); search != command_mapper.end())
    {
        cmd = search->second;
    }

    switch (cmd)
    {
        case Command::decode:
            return process_decode(argc, argv);
        case Command::info:
            return process_info(argc, argv);
        case Command::peers:
            return process_peers(argc, argv);
        default:
            std::cerr << "Unknown command: " << command << std::endl;
            return 1;
    }
}