
#ifndef _COAP_PACKET_HPP_
#define _COAP_PACKET_HPP_

#include <Arduino.h>
#include "CoapConfig.hpp"
#include "CoapDataStructures.hpp"

namespace Coap
{

    enum OptionNumber
    {
        ON_IF_MATCH = 1,
        ON_URI_HOST = 3,
        ON_E_TAG = 4,
        ON_IF_NONE_MATCH = 5,
        ON_URI_PORT = 7,
        ON_LOCATION_PATH = 8,
        ON_URI_PATH = 11,
        ON_CONTENT_FORMAT = 12,
        ON_MAX_AGE = 14,
        ON_URI_QUERY = 15,
        ON_ACCEPT = 17,
        ON_LOCATION_QUERY = 20,
        ON_PROXY_URI = 35,
        ON_PROXY_SCHEME = 39
    };

    class Option
    {
    public:
    Option()
    : number(0),length(0),buffer(nullptr) {}
        uint8_t number;
        uint8_t length;
        uint8_t *buffer;
    };

    class Packet
    {
    public:
        Packet();
        Type type;
        uint8_t code;
        const uint8_t *token;
        uint8_t tokenLength;
        const uint8_t *payload;
        size_t payloadLength;
        uint16_t msgId;
        uint8_t optionCount;
        Option options[CoapConfig::MaxOptions];

        bool addOption(uint8_t number, uint8_t length, uint8_t *opt_payload);
    };
};

#endif