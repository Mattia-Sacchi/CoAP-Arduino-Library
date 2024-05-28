#ifndef _COAP_RESOURCE_HPP_
#define _COAP_RESOURCE_HPP_

#include "CoapPacket.hpp"
#include "CoapDataStructures.hpp"
#include <Arduino.h>

namespace Coap{

struct ResponseInfo
{
  ResponseInfo() : payload(""), code(RC_NOT_IMPLEMENTED), type(CT_NONE) {}
  String payload;
  ResponseCode code;
  ContentType type;
};

class Resource
{
public:
  Resource(String topic);
  virtual void onCalled(Packet &packet, IPAddress ip, int port) {}
  ResponseInfo getResponseInfo() const { return m_responseInfo; }
  inline String topic() const { return m_topic; }

private:
  String m_topic;

protected:
  ResponseInfo m_responseInfo;
};

};

#endif