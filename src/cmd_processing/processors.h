#pragma once

#include <iostream>

#include "../lib/bencode/decode.h"
#include "../lib/bencode/encode.h"
#include "../lib/torrent/utils.h"

// TODO: add descr
int process_decode(int argc, char* argv[]);

int process_info(int argc, char* argv[]);

int process_peers(int argc, char* argv[]);

int process_handshake(int argc, char* argv[]);

int process_download_piece(int argc, char* argv[]);