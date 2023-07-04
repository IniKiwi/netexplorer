#include "tcp.h"
#include <cstring>
#include <unistd.h>

#include "../logger.h"
#include "../lapi.h"

int tcp_connect(Ipv4Addr addr, size_t timeout){
    std::string ip_str = addr.to_string_ip();

    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct timeval _timeout;      
    _timeout.tv_sec = 0;
    _timeout.tv_usec = timeout;
    
    setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, &_timeout, sizeof(_timeout));
    setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, &_timeout, sizeof(_timeout));

    struct sockaddr_in server;
    std::memset(&server,0,sizeof(struct sockaddr_in));
    server.sin_addr.s_addr = inet_addr(addr.to_string_ip().c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(addr.port);
    int ret = -1;
    if(connect(sockfd, (struct sockaddr*)&server, sizeof(server)) != -1){
        ret = sockfd;
    }
    else{
        close(sockfd);
    }
    return ret;
}

int tcp_action(Ipv4Addr addr, NetworkTask* task){
    addr.protocol = "tcp";
    int result = tcp_connect(addr, task->get_timeout());
    if(result != -1){
        int callback = task->get_lua_protocol_callback("tcp");
        if(callback != 0){
            std::mutex lock;
            lock.lock();
            lua_State* L = task->get_lua_state();
            lua_rawgeti( L, LUA_REGISTRYINDEX, callback);
            l_push_ipv4addr(L, &addr);
            lua_pcall( L, 1, 0, 0 );
            lock.unlock();
        }
        task->get_logger()->log_request(RequestStatus::OK, addr, "");
        task->push_raw_result(addr);
        close(result);
        return RequestStatus::OK;
    }
    task->get_logger()->log_request(RequestStatus::FAIL, addr, "");
    return RequestStatus::FAIL;
}