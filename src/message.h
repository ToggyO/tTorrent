#pragma once


#include <cstdint>
#include <vector>

// TODO: add descr
struct PeerMessage
{
    uint32_t length;
    uint8_t id;

//    static PeerMessage parse()
//    {
//        return
//    }

} __attribute__((packed));

struct BitfieldPeerMessage : public PeerMessage
{
    std::vector<uint8_t> payload;
};

struct RequestPeerMessage : public PeerMessage
{
    uint32_t index;
    uint32_t begin;
    uint32_t length_block;
} __attribute__((packed));