#pragma once

#include <string>
#include <unordered_map>

const static std::string k_decode_cmd_name = "decode";
const static std::string k_info_cmd_name = "info";
const static std::string k_peers_cmd_name = "peers";

enum class Command
{
    unknown,
    decode,
    info,
    peers
};

/** @brief Command literal names mapper. */
std::unordered_map<std::string, Command> command_mapper = {
    { k_decode_cmd_name, Command::decode },
    { k_info_cmd_name, Command::info },
    { k_peers_cmd_name, Command::peers }
};