#ifndef _COAP_URI_HPP_
#define _COAP_URI_HPP_

#include "CoapResource.hpp"
#include <Arduino.h>
#include <map>



namespace Coap
{

class Uri
{
private:

    
    std::map<String,Resource*> m_resources;

public:
    Uri();
    void add(Resource &resource, String url);
    Resource * find(String url);
};

};
#endif