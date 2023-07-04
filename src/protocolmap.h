#pragma once

#include <map>
#include <string>

#define PROTOCOLMAP_STR "ProtocolMap"
class ProtocolMap{
    private:
    std::map<uint16_t,std::string> m_protocol_map;
    std::string m_protocol_default = "tcp";
    public:

    void clear();
    void import(std::string filename);

    std::string get(uint16_t port);
    void set(uint16_t port, std::string entry);
};