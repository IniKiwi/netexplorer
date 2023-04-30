#include "tcp.h"
#include <cstring>
#include <unistd.h>

#include "../logger.h"

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
        task->get_logger()->log_request(RequestStatus::OK, addr, "");
        task->push_raw_result(addr);
        close(result);
        return RequestStatus::OK;
    }
    task->get_logger()->log_request(RequestStatus::FAIL, addr, "");
    return RequestStatus::FAIL;
}