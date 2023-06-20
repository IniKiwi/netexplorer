#include "ftp.h"
#include "tcp.h"

#include <cstring>
#include <unistd.h>

#include "../logger.h"
#include "../config.h"

int ftp_login(int sockfd, const char* username, const char* password, char* buf, std::string &log){
    int ret = 0;
    sprintf(buf, "USER %s\n", username);
    send(sockfd, buf, strlen(buf), 0);
    log += COLOR_BLUE;
    log += buf;
    bzero(buf, MIN_BUFFER_SIZE);
    recv(sockfd, buf, MIN_BUFFER_SIZE, 0);
    if(buf[0] != '3' || buf[1] != '3' || buf[2] != '1'){
        log += COLOR_RED;
        ret = -1;
    }
    else
        log += COLOR_GREEN;
    
    log += buf;
    
    sprintf(buf, "PASS %s\n", password);
    log += COLOR_BLUE;
    log += buf;
    send(sockfd, buf, strlen(buf), 0);

    bzero(buf, MIN_BUFFER_SIZE);
    recv(sockfd, buf, MIN_BUFFER_SIZE, 0);
    if(buf[0] != '2' || buf[1] != '3' || buf[2] != '0'){
        log += COLOR_RED;
        ret = -1;
    }
    else
        log += COLOR_GREEN;
    
    log += buf;
    return ret;
}

int ftp_pasv(int sockfd, Ipv4Addr addr, NetworkTask* task, char* buf, std::string &log){
    int ret = 0;
    sprintf(buf, "PASV\n");
    send(sockfd, buf, strlen(buf), 0);
    log += COLOR_BLUE;
    log += buf;
    bzero(buf, MIN_BUFFER_SIZE);
    recv(sockfd, buf, MIN_BUFFER_SIZE, 0);
    if(buf[0] != '2' || buf[1] != '2' || buf[2] != '7'){
        log += COLOR_RED;
        ret = -1;
    }
    else
        log += COLOR_GREEN;
    
    log += buf;
    if(ret == -1){return ret;}

    char* h1 = strchr(buf, '(')+1;
    char* h2 = strchr(h1, ',')+1;
    char* h3 = strchr(h2, ',')+1;
    char* h4 = strchr(h3, ',')+1;
    char* p1 = strchr(h4, ',')+1;
    char* p2 = strchr(p1, ',')+1;

    Ipv4Addr dataconn;
    dataconn.ip[0].i = std::stoi(h1);
    dataconn.ip[1].i = std::stoi(h2);
    dataconn.ip[2].i = std::stoi(h3);
    dataconn.ip[3].i = std::stoi(h4);
    dataconn.port = (std::stoi(p1)*256)+std::stoi(p2);
    return tcp_connect(dataconn, task->get_timeout());
}

int ftp_list(int sockfd, Ipv4Addr addr, NetworkTask* task, char* buf, std::string &log){
    int datafd = ftp_pasv(sockfd, addr, task, buf, log);
    if(datafd == -1) return -1;

    int ret = 0;
    /* set type A (ascii)*/
    sprintf(buf, "TYPE A\n");
    send(sockfd, buf, strlen(buf), 0);
    log += COLOR_BLUE;
    log += buf;

    bzero(buf, MIN_BUFFER_SIZE);
    recv(sockfd, buf, MIN_BUFFER_SIZE, 0);

    log += COLOR_GREEN;
    log += buf;

    /* list files */
    sprintf(buf, "LIST\n");
    send(sockfd, buf, strlen(buf), 0);
    log += COLOR_BLUE;
    log += buf;

    bzero(buf, MIN_BUFFER_SIZE);
    recv(datafd, buf, MIN_BUFFER_SIZE, 0);

    log += "\e[35m";
    log += buf;

    close(datafd);
    return 0;
}

int ftp_action(Ipv4Addr addr, NetworkTask* task){
    int sockfd = tcp_connect(addr, task->get_timeout());
    if(sockfd == -1){
        task->get_logger()->log_request(RequestStatus::FAIL, addr, "");
        return RequestStatus::FAIL;
    }
    char* buf = (char*)malloc(MIN_BUFFER_SIZE);
    if(buf == NULL){
        task->get_logger()->log_request(RequestStatus::FAIL, addr, "");
        return RequestStatus::FAIL;
    }
    recv(sockfd, buf, MIN_BUFFER_SIZE, 0);
    buf[MIN_BUFFER_SIZE] = '\0';
    if(buf[0] != '2' || buf[1] != '2' || buf[2] != '0'){
        task->get_logger()->log_request(RequestStatus::FAIL, addr, "");
        free(buf);
        close(sockfd);
        return RequestStatus::FAIL;
    }
    std::vector<std::string> log;
    char* p = strchr(buf, '\n');
    if(p == NULL) log.push_back(std::string(buf));
    else log.push_back("server: "+std::string(buf, p-buf));

    std::string login;
    int logged = ftp_login(sockfd, "anonymous", "********", buf, login);
    log.push_back("login: "+login);

    if(logged == -1){
        task->get_logger()->log_request(RequestStatus::ACCESS_DENIED, addr, "", log);
        free(buf);
        close(sockfd);
        return RequestStatus::ACCESS_DENIED;
    }

    std::string ls;
    ftp_list(sockfd, addr, task, buf, ls);
    log.push_back("ls: "+ls);
    

    task->get_logger()->log_request(RequestStatus::OK, addr, "", log);
    free(buf);
    close(sockfd);
    return RequestStatus::OK;
}