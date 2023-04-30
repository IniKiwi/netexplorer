#pragma once

#include "../config.h"
#include "../networktask.h"

enum HttpResultCode{
    HTTP_OK,
    HTTP_FILE_TOO_BIG,
    HTTP_INVALID,
    HTTP_NET_ERR,
};

class HttpRequestResult{
    private:
    char* m_raw_data;
    size_t m_size;
    Ipv4Addr m_addr;
    std::string m_path;
    public:
    HttpRequestResult(Ipv4Addr addr, std::string path, char* buffer, size_t size): m_raw_data(buffer), m_size(size), m_addr(addr), m_path(path) {}
    ~HttpRequestResult(){
        std::free(m_raw_data);
    }

    std::string get_status();
    std::string get_version();
    std::string get_full_header();
    std::string get_header(std::string key);
    inline bool is_http();
};

HttpRequestResult* http_get(int sockfd, Ipv4Addr addr, std::string path);
int http_action(Ipv4Addr addr, std::string path, NetworkTask* task);