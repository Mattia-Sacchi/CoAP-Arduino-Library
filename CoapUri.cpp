#include "CoapUri.hpp"
#include "CoapConfig.hpp"

using namespace Coap;

Uri::Uri(){};

void Uri::add(Resource &resource, String url)
{
    m_resources[url] = &resource;
};

Resource *Uri::find(String url)
{
    return m_resources[url];
};