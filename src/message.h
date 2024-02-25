#pragma once


#include <cstdint>
#include <vector>

// TODO: add descr
enum class MessageId : uint8_t
{
    unknown = 100,
    /** @brief chokes the receiver */
    choke = 0,
    /** @brief unchokes the receiver */
    unchoke,
    /** @brief expresses interest in receiving data */
    interested,
    /** @brief expresses disinterest in receiving data */
    not_interested,
    /** @brief alerts the receiver that the sender has downloaded a piece */
    have,
    /** @brief encodes which pieces that the sender has downloaded */
    bitfield,
    /** @brief requests a block of data from the receiver */
    request,
    /** @brief delivers a block of data to fulfill a request */
    piece,
    /** @brief cancels a request */
    cancel
};

// TODO: add descr
struct Message
{
    uint32_t length;
    MessageId id;

    Message() : Message(0, MessageId::unknown) {}

    virtual std::vector<uint8_t> serialize()
    {
        std::vector<uint8_t> buffer;
        buffer.reserve(5);

        split_uint32_into_bytes(buffer, length);
        buffer.push_back((uint8_t)id);

        return buffer;
    }

    virtual void deserialize(const std::vector<uint8_t>& bytes)
    {
//        length = ((uint32_t)bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0]; TODO: remove
        length = ((uint32_t)bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
        id = MessageId(bytes[4]);
    }

protected:
    Message(uint32_t len, MessageId id) : length{len}, id{id} {}

    static void split_uint32_into_bytes(std::vector<uint8_t>& bytes, const uint32_t& num)
    {
        auto num_ptr = (uint8_t*) &num;
        for (int i = 0; i < 4; ++i)
        {
            bytes.push_back(*(num_ptr + i));
        }
    }

} __attribute__((packed));

struct UnchokeMessage : public Message
{
    UnchokeMessage() : Message(1, MessageId::unchoke) {}
};

struct InterestedMessage : public Message
{
    InterestedMessage() : Message(1, MessageId::interested) {}
};

struct BitfieldMessage : public Message
{
    std::vector<uint8_t> payload; // TODO: check __packed__

    std::vector<uint8_t> serialize() override
    {
        auto buf = Message::serialize();
        buf.reserve(buf.size() + payload.size());

        for (const auto& byte : payload)
        {
            buf.push_back(byte);
        }

        return buf;
    }

    void deserialize(const std::vector<uint8_t>& bytes) override
    {
        Message::deserialize(bytes);

        auto begin = bytes.begin();
        payload = std::vector<uint8_t>(begin + 5, begin + 5 + (length - 1));
    }

} __attribute__((packed));

struct RequestMessage : public Message
{
    uint32_t index;
    uint32_t begin;
    uint32_t length_block;

    std::vector<uint8_t> serialize() override
    {
        auto buf = Message::serialize();
        buf.reserve(buf.size() + sizeof(index) + sizeof(begin) + sizeof(length_block));

        split_uint32_into_bytes(buf, index);
        split_uint32_into_bytes(buf, begin);
        split_uint32_into_bytes(buf, length_block);

        return buf;
    }

} __attribute__((packed));


template <class TMessage, typename = std::enable_if_t<std::is_base_of_v<Message, TMessage>>>
inline deserialize_message(const std::vector<uint8_t>& bytes)
{

    auto length = ((uint32_t)bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    auto id = MessageId(bytes[4]);

    switch (id)
    {
        case MessageId::choke:
            m_choked = true;
            break;
        case MessageId::unchoke:
            m_choked = false;
            break;
        case MessageId::bitfield:
//                m_bitfield = std::move((*static_cast<BitfieldMessage*>(&msg)).payload);
            break;
        case MessageId::have:
            // TODO:
            break;
        default:
            // TODO:
            break;
    }

}