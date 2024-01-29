#pragma once

#include <string>
#include <unordered_map>

const static std::string k_decode_cmd_name = "decode";
const static std::string k_info_cmd_name = "info";
const static std::string k_peers_cmd_name = "peers";
const static std::string k_handshake_cmd_name = "handshake";
const static std::string k_download_piece_cmd_name = "download_piece";

enum class Command
{
    unknown,
    decode,
    info,
    peers,
    handshake,
    download_piece
};

/** @brief Command literal names mapper. */
std::unordered_map<std::string, Command> command_mapper = {
    { k_decode_cmd_name, Command::decode },
    { k_info_cmd_name, Command::info },
    { k_peers_cmd_name, Command::peers },
    { k_handshake_cmd_name, Command::handshake },
    { k_download_piece_cmd_name, Command::download_piece }
};