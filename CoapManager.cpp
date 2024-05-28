#include "CoapManager.hpp"
#include "Arduino.h"
#include "CoapConfig.hpp"

using namespace Coap;

Manager::Manager(
    UDP &udp,
    int coap_buf_size /* default value is CoapConfig::MaxBufferSize */
)
{
    this->_udp = &udp;
    this->coap_buf_size = coap_buf_size;
    this->tx_buffer = new uint8_t[this->coap_buf_size];
    this->rx_buffer = new uint8_t[this->coap_buf_size];
}

Manager::~Manager()
{
    if (this->tx_buffer != NULL)
        delete[] this->tx_buffer;

    if (this->rx_buffer != NULL)
        delete[] this->rx_buffer;
}

bool Manager::start()
{
    this->start(CoapConfig::DefaultCoapPort);
    return true;
}

bool Manager::start(int port)
{
    this->_udp->begin(port);
    return true;
}

uint16_t Manager::sendPacket(Packet &packet, IPAddress ip)
{
    return this->sendPacket(packet, ip, CoapConfig::DefaultCoapPort);
}

uint16_t Manager::sendPacket(Packet &packet, IPAddress ip, int port)
{
    uint8_t *p = this->tx_buffer;
    uint16_t running_delta = 0;
    uint16_t packetSize = 0;

    // make coap packet base header
    *p = 0x01 << 6;
    *p |= (packet.type & 0x03) << 4;
    *p++ |= (packet.tokenLength & 0x0F);
    *p++ = packet.code;
    *p++ = (packet.msgId >> 8);
    *p++ = (packet.msgId & 0xFF);
    p = this->tx_buffer + CoapConfig::HeaderSize;
    packetSize += 4;

    // make token
    if (packet.token != NULL && packet.tokenLength <= 0x0F)
    {
        memcpy(p, packet.token, packet.tokenLength);
        p += packet.tokenLength;
        packetSize += packet.tokenLength;
    }

    // make option header
    for (int i = 0; i < packet.optionCount; i++)
    {
        uint32_t optdelta;
        uint8_t len, delta;

        if (packetSize + 5 + packet.options[i].length >= coap_buf_size)
        {
            return 0;
        }
        optdelta = packet.options[i].number - running_delta;
        delta = OptionDelta(optdelta);
        len = OptionDelta((uint32_t)packet.options[i].length);

        *p++ = (0xFF & (delta << 4 | len));
        if (delta == 13)
        {
            *p++ = (optdelta - 13);
            packetSize++;
        }
        else if (delta == 14)
        {
            *p++ = ((optdelta - 269) >> 8);
            *p++ = (0xFF & (optdelta - 269));
            packetSize += 2;
        }
        if (len == 13)
        {
            *p++ = (packet.options[i].length - 13);
            packetSize++;
        }
        else if (len == 14)
        {
            *p++ = (packet.options[i].length >> 8);
            *p++ = (0xFF & (packet.options[i].length - 269));
            packetSize += 2;
        }

        memcpy(p, packet.options[i].buffer, packet.options[i].length);
        p += packet.options[i].length;
        packetSize += packet.options[i].length + 1;
        running_delta = packet.options[i].number;
    }

    // make payload
    if (packet.payloadLength > 0)
    {
        if ((packetSize + 1 + packet.payloadLength) >= coap_buf_size)
        {
            return 0;
        }
        *p++ = 0xFF;
        memcpy(p, packet.payload, packet.payloadLength);
        packetSize += 1 + packet.payloadLength;
    }

    _udp->beginPacket(ip, port);
    _udp->write(this->tx_buffer, packetSize);
    _udp->endPacket();

    return packet.msgId;
}

uint16_t Manager::get(IPAddress ip, int port, const char *url)
{
    return this->send(ip, port, url, T_CON, M_GET, NULL, 0, NULL, 0);
}

uint16_t Manager::put(IPAddress ip, int port, const char *url, const char *payload)
{
    return this->send(ip, port, url, T_CON, M_PUT, NULL, 0, (uint8_t *)payload, strlen(payload));
}

uint16_t Manager::put(IPAddress ip, int port, const char *url, const char *payload, size_t payloadlen)
{
    return this->send(ip, port, url, T_CON, M_PUT, NULL, 0, (uint8_t *)payload, payloadlen);
}

uint16_t Manager::send(IPAddress ip, int port, const char *url, Type type, Method method, const uint8_t *token, uint8_t tokenlen, const uint8_t *payload, size_t payloadlen)
{
    return this->send(ip, port, url, type, method, token, tokenlen, payload, payloadlen, CT_NONE);
}

uint16_t Manager::send(IPAddress ip, int port, const char *url, Type type, Method method, const uint8_t *token, uint8_t tokenlen, const uint8_t *payload, size_t payloadlen, ContentType content_type)
{
    return this->send(ip, port, url, type, method, token, tokenlen, payload, payloadlen, content_type, rand());
}

uint16_t Manager::send(IPAddress ip, int port, const char *url, Type type, Method method, const uint8_t *token, uint8_t tokenlen, const uint8_t *payload, size_t payloadlen, ContentType content_type, uint16_t messageid)
{

    // make packet
    Packet packet;

    packet.type = type;
    packet.code = method;
    packet.token = token;
    packet.tokenLength = tokenlen;
    packet.payload = payload;
    packet.payloadLength = payloadlen;
    packet.optionCount = 0;
    packet.msgId = messageid;

    // use URI_HOST UIR_PATH
    char ipaddress[16] = "";
    sprintf(ipaddress, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    packet.addOption(ON_URI_HOST, strlen(ipaddress), (uint8_t *)ipaddress);

    /*
        Add Query Support
        Author: @YelloooBlue
    */

    // parse url
    size_t idx = 0;
    bool hasQuery = false;
    for (size_t i = 0; i < strlen(url); i++)
    {
        // The reserved characters "/"  "?"  "&"
        if (url[i] == '/')
        {
            packet.addOption(ON_URI_PATH, i - idx, (uint8_t *)(url + idx)); // one URI_PATH (terminated by '/')
            idx = i + 1;
        }
        else if (url[i] == '?' && !hasQuery)
        {
            packet.addOption(ON_URI_PATH, i - idx, (uint8_t *)(url + idx)); // the last URI_PATH (between / and ?)
            hasQuery = true;                                                  // now start to parse the query
            idx = i + 1;
        }
        else if (url[i] == '&' && hasQuery)
        {
            packet.addOption(ON_URI_QUERY, i - idx, (uint8_t *)(url + idx)); // one URI_QUERY (terminated by '&')
            idx = i + 1;
        }
    }

    if (idx <= strlen(url))
    {
        if (hasQuery)
        {
            packet.addOption(ON_URI_QUERY, strlen(url) - idx, (uint8_t *)(url + idx)); // the last URI_QUERY (between &/? and the end)
        }
        else
        {
            packet.addOption(ON_URI_PATH, strlen(url) - idx, (uint8_t *)(url + idx)); // the last URI_PATH (between / and the end)
        }
    }

    /*
        Adding query support ends
        Date: 2024.03.03
    */

    // if Content-Format option
    uint8_t optionBuffer[2]{0};
    if (content_type != CT_NONE)
    {
        optionBuffer[0] = ((uint16_t)content_type & 0xFF00) >> 8;
        optionBuffer[1] = ((uint16_t)content_type & 0x00FF);
        packet.addOption(ON_CONTENT_FORMAT, 2, optionBuffer);
    }

    // send packet
    return this->sendPacket(packet, ip, port);
}

int Manager::parseOption(Option *option, uint16_t *running_delta, uint8_t **buf, size_t buflen)
{
    uint8_t *p = *buf;
    uint8_t headlen = 1;
    uint16_t len, delta;

    if (buflen < headlen)
        return -1;

    delta = (p[0] & 0xF0) >> 4;
    len = p[0] & 0x0F;

    if (delta == 13)
    {
        headlen++;
        if (buflen < headlen)
            return -1;
        delta = p[1] + 13;
        p++;
    }
    else if (delta == 14)
    {
        headlen += 2;
        if (buflen < headlen)
            return -1;
        delta = ((p[1] << 8) | p[2]) + 269;
        p += 2;
    }
    else if (delta == 15)
        return -1;

    if (len == 13)
    {
        headlen++;
        if (buflen < headlen)
            return -1;
        len = p[1] + 13;
        p++;
    }
    else if (len == 14)
    {
        headlen += 2;
        if (buflen < headlen)
            return -1;
        len = ((p[1] << 8) | p[2]) + 269;
        p += 2;
    }
    else if (len == 15)
        return -1;

    if ((p + 1 + len) > (*buf + buflen))
        return -1;
    option->number = delta + *running_delta;
    option->buffer = p + 1;
    option->length = len;
    *buf = p + 1 + len;
    *running_delta += delta;

    return 0;
}

bool Manager::loop()
{
    int32_t packetlen = _udp->parsePacket();

    while (packetlen > 0)
    {
        packetlen = _udp->read(this->rx_buffer, packetlen >= coap_buf_size ? coap_buf_size : packetlen);

        Packet packet;

        // parse coap packet header
        if (packetlen < CoapConfig::HeaderSize || (((this->rx_buffer[0] & 0xC0) >> 6) != 1))
        {
            packetlen = _udp->parsePacket();
            continue;
        }

        packet.type = static_cast<Type>((this->rx_buffer[0] & 0x30) >> 4);
        packet.tokenLength = this->rx_buffer[0] & 0x0F;
        packet.code = static_cast<Method>(this->rx_buffer[1]);
        packet.msgId = 0xFF00 & (this->rx_buffer[2] << 8);
        packet.msgId |= 0x00FF & this->rx_buffer[3];

        if (packet.tokenLength == 0)
            packet.token = NULL;
        else if (packet.tokenLength <= 8)
            packet.token = this->rx_buffer + 4;
        else
        {
            packetlen = _udp->parsePacket();
            continue;
        }

        // parse packet options/payload
        if (CoapConfig::HeaderSize + packet.tokenLength < packetlen)
        {
            int optionIndex = 0;
            uint16_t delta = 0;
            uint8_t *end = this->rx_buffer + packetlen;
            uint8_t *p = this->rx_buffer + CoapConfig::HeaderSize + packet.tokenLength;
            while (optionIndex < CoapConfig::MaxOptions && *p != 0xFF && p < end)
            {
                // packet.options[optionIndex];
                if (0 != parseOption(&packet.options[optionIndex], &delta, &p, end - p))
                    return false;
                optionIndex++;
            }
            packet.optionCount = optionIndex;

            if (p + 1 < end && *p == 0xFF)
            {
                packet.payload = p + 1;
                packet.payloadLength = end - (p + 1);
            }
            else
            {
                packet.payload = NULL;
                packet.payloadLength = 0;
            }
        }

 

        String url = "";
        // call endpoint url function
        for (int i = 0; i < packet.optionCount; i++)
        {
            if (packet.options[i].number == ON_URI_PATH && packet.options[i].length > 0)
            {
                char urlname[packet.options[i].length + 1];
                memcpy(urlname, packet.options[i].buffer, packet.options[i].length);
                urlname[packet.options[i].length] = 0;
                if (url.length() > 0)
                    url += "/";
                url += (const char *)urlname;
            }
        }

        Resource *temp = uri.find(url);

        if (!!temp)
        {
            temp->onCalled(packet, _udp->remoteIP(), _udp->remotePort());
            ResponseInfo rInfo = temp->getResponseInfo();
            String payloadStr = rInfo.payload;
            sendResponse(_udp->remoteIP(), _udp->remotePort(), packet.msgId, payloadStr.c_str(), payloadStr.length(), rInfo.code, rInfo.type, packet.token, packet.tokenLength);
        }
        else
        {

            sendResponse(_udp->remoteIP(), _udp->remotePort(), packet.msgId, NULL, 0,
                            RC_NOT_FOUNT, CT_NONE, NULL, 0);
        }
        

        /* this type check did not use.
        if (packet.type == COAP_CON) {
            // send response
             sendResponse(_udp->remoteIP(), _udp->remotePort(), packet.messageid);
        }
         */

        // next packet
        packetlen = _udp->parsePacket();
    }

    return true;
}

uint16_t Manager::sendResponse(IPAddress ip, int port, uint16_t messageid)
{
    return this->sendResponse(ip, port, messageid, NULL, 0, RC_CONTENT, CT_TEXT_PLAIN, NULL, 0);
}

uint16_t Manager::sendResponse(IPAddress ip, int port, uint16_t messageid, const char *payload)
{
    return this->sendResponse(ip, port, messageid, payload, strlen(payload), RC_CONTENT, CT_TEXT_PLAIN, NULL, 0);
}

uint16_t Manager::sendResponse(IPAddress ip, int port, uint16_t messageid, const char *payload, size_t payloadlen)
{
    return this->sendResponse(ip, port, messageid, payload, payloadlen, RC_CONTENT, CT_TEXT_PLAIN, NULL, 0);
}

uint16_t Manager::sendResponse(IPAddress ip, int port, uint16_t messageid, const char *payload, size_t payloadlen,
                               Coap::ResponseCode code, ContentType type, const uint8_t *token, int tokenlen)
{
    // make packet
    Packet packet;

    packet.type = T_ACK;
    packet.code = code;
    packet.token = token;
    packet.tokenLength = tokenlen;
    packet.payload = (uint8_t *)payload;
    packet.payloadLength = payloadlen;
    packet.optionCount = 0;
    packet.msgId = messageid;

    // if more options?
    uint8_t optionBuffer[2] = {0};
    optionBuffer[0] = ((uint16_t)type & 0xFF00) >> 8;
    optionBuffer[1] = ((uint16_t)type & 0x00FF);
    packet.addOption(ON_CONTENT_FORMAT, 2, optionBuffer);

    return this->sendPacket(packet, ip, port);
}
