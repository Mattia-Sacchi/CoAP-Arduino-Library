#include "CoapPacket.hpp"
using namespace Coap;

bool Packet::addOption(uint8_t number, uint8_t length, uint8_t *opt_payload)
{
    if (optionCount >= CoapConfig::MaxOptions)
        return false;
    options[optionCount].number = number;
    options[optionCount].length = length;
    options[optionCount].buffer = opt_payload;

    ++optionCount;
    return true;
}

Packet::Packet()
: type(T_RESET),code(M_GET),token(nullptr),tokenLength(0),payload(nullptr),payloadLength(0),msgId(0),optionCount(0)

{
}