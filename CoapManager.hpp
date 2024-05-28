/*
CoAP library for Arduino.
This software is released under the MIT License.
Copyright (c) 2014 Hirotaka Niisato
Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef __SIMPLE_COAP_H__
#define __SIMPLE_COAP_H__

#include "Udp.h"
#include "CoapResource.hpp"
#include "CoapPacket.hpp"
#include "CoapUri.hpp"
#include "CoapDataStructures.hpp"
#include "CoapConfig.hpp"

namespace Coap
{
 
    class Manager
    {
    private:
        UDP *_udp;
        Uri uri;
        int _port;
        int coap_buf_size;
        uint8_t *tx_buffer = NULL;
        uint8_t *rx_buffer = NULL;

        uint16_t sendPacket(Packet &packet, IPAddress ip);
        uint16_t sendPacket(Packet &packet, IPAddress ip, int port);
        int parseOption(Option *option, uint16_t *running_delta, uint8_t **buf, size_t buflen);

    public:
        Manager(
            UDP &udp,
            int coap_buf_size = CoapConfig::MaxBufferSize);
        ~Manager();
        bool start();
        bool start(int port);

        void server(Resource &c) { uri.add(c, c.topic()); }
        uint16_t sendResponse(IPAddress ip, int port, uint16_t messageid);
        uint16_t sendResponse(IPAddress ip, int port, uint16_t messageid, const char *payload);
        uint16_t sendResponse(IPAddress ip, int port, uint16_t messageid, const char *payload, size_t payloadlen);
        uint16_t sendResponse(IPAddress ip, int port, uint16_t messageid, const char *payload, size_t payloadlen, ResponseCode code, ContentType type, const uint8_t *token, int tokenlen);

        uint16_t get(IPAddress ip, int port, const char *url);
        uint16_t put(IPAddress ip, int port, const char *url, const char *payload);
        uint16_t put(IPAddress ip, int port, const char *url, const char *payload, size_t payloadlen);
        uint16_t send(IPAddress ip, int port, const char *url, Type type, Method method, const uint8_t *token, uint8_t tokenlen, const uint8_t *payload, size_t payloadlen);
        uint16_t send(IPAddress ip, int port, const char *url, Type type, Method method, const uint8_t *token, uint8_t tokenlen, const uint8_t *payload, size_t payloadlen, ContentType content_type);
        uint16_t send(IPAddress ip, int port, const char *url, Type type, Method method, const uint8_t *token, uint8_t tokenlen, const uint8_t *payload, size_t payloadlen, ContentType content_type, uint16_t messageid);

        bool loop();
    };

};

#endif
