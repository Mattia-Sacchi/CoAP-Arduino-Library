#ifndef _COAP_DATA_STRUCTURES_HPP_
#define _COAP_DATA_STRUCTURES_HPP_

namespace Coap
{

    inline uint8_t OptionDelta(uint32_t value)
    {
        return value < 13 ? (0xFF & value) : (value <= 0xFF + 13 ? 13 : 14);
    }

    enum Type
    {
        T_CON = 0,
        T_NONCON = 1,
        T_ACK = 2,
        T_RESET = 3
    };

    enum Method
    {
        M_GET = 1,
        M_POST = 2,
        M_PUT = 3,
        M_DELETE = 4
    };

#define RESPONSE_CODE(class, detail) ((class << 5) | (detail))

    enum ResponseCode
    {
        RC_CREATED = RESPONSE_CODE(2, 1),
        RC_DELETED = RESPONSE_CODE(2, 2),
        RC_VALID = RESPONSE_CODE(2, 3),
        RC_CHANGED = RESPONSE_CODE(2, 4),
        RC_CONTENT = RESPONSE_CODE(2, 5),
        RC_BAD_REQUEST = RESPONSE_CODE(4, 0),
        RC_UNAUTHORIZED = RESPONSE_CODE(4, 1),
        RC_BAD_OPTION = RESPONSE_CODE(4, 2),
        RC_FORBIDDEN = RESPONSE_CODE(4, 3),
        RC_NOT_FOUNT = RESPONSE_CODE(4, 4),
        RC_METHOD_NOT_ALLOWD = RESPONSE_CODE(4, 5),
        RC_NOT_ACCEPTABLE = RESPONSE_CODE(4, 6),
        RC_PRECONDITION_FAILED = RESPONSE_CODE(4, 12),
        RC_REQUEST_ENTITY_TOO_LARGE = RESPONSE_CODE(4, 13),
        RC_UNSUPPORTED_CONTENT_FORMAT = RESPONSE_CODE(4, 15),
        RC_INTERNAL_SERVER_ERROR = RESPONSE_CODE(5, 0),
        RC_NOT_IMPLEMENTED = RESPONSE_CODE(5, 1),
        RC_BAD_GATEWAY = RESPONSE_CODE(5, 2),
        RC_SERVICE_UNAVALIABLE = RESPONSE_CODE(5, 3),
        RC_GATEWAY_TIMEOUT = RESPONSE_CODE(5, 4),
        RC_PROXYING_NOT_SUPPORTED = RESPONSE_CODE(5, 5)
    };

    enum ContentType
    {
        CT_NONE = -1,
        CT_TEXT_PLAIN = 0,
        CT_APPLICATION_LINK_FORMAT = 40,
        CT_APPLICATION_XML = 41,
        CT_APPLICATION_OCTET_STREAM = 42,
        CT_APPLICATION_EXI = 47,
        CT_APPLICATION_JSON = 50,
        CT_APPLICATION_CBOR = 60
    };

};

#endif