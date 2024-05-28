#ifndef _COAP_CONFIGS_HPP_
#define _COAP_CONFIGS_HPP_

#include "Arduino.h"ì

namespace CoapConfig
{
    static const uint32_t DefaultCoapPort = 5683;

    static const uint32_t DefaultCoapSecurePort = 5684;
    static const uint16_t MaxBufferSize = 128;
    static const uint8_t HeaderSize = 4;
    static const uint8_t OptionHeaderSize = 1; // Unused
    static const uint8_t PayloadMarker = 0xff; // Unused
    static const uint8_t MaxOptions = 10;

};

#endif