#include "http.h"
#include "tcp.h"

#include "../logger.h"
#include "../exceptions.h"

#include "../parser/html.h"

#include <cstring>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <unistd.h>

HttpRequestResult* http_get(int sockfd, Ipv4Addr addr, std::string path){
    char* req_buffer = (char*)std::malloc(MIN_BUFFER_SIZE);
    std::snprintf(req_buffer, MIN_BUFFER_SIZE, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path.c_str(), addr.get_host());

    size_t http_req_s = std::strlen(req_buffer);
    //char* req_buffer_ptr = req_buffer;

    size_t sent = 0;
    while(sent < http_req_s){
        int bytes = send(sockfd,req_buffer+sent,http_req_s-sent, 0);
        if(bytes < 0){
            throw NetworkError(strerror(errno));
        }
        if(bytes == 0){
            break;
        }
        sent += bytes;
    };
    std::free(req_buffer);

    char* result_buffer = NULL;
    char* recv_buffer = new char[MIN_BUFFER_SIZE];
    std::memset(recv_buffer, 0, MIN_BUFFER_SIZE);
    size_t received = 0;
    while(1){
        int bytes = recv(sockfd,recv_buffer,MIN_BUFFER_SIZE-1, 0);
        if (bytes < 0 ){
            //int err = errno;
            delete recv_buffer;
            //std::free(result_buffer);
            throw NetworkError(strerror(errno));
        }
        if (bytes == 0){
            break;
        }
        result_buffer = reinterpret_cast<char*>(std::realloc(result_buffer, received+bytes));
        std::memcpy(&result_buffer[received],recv_buffer, bytes);
        received+=bytes;

        if(received > 5000000){
            delete recv_buffer;
            std::free(result_buffer);
            throw DataException("recevied more than 5MB.");
        }
    };
    delete recv_buffer;
    return new HttpRequestResult(addr, path, result_buffer, received);
}

int http_action(Ipv4Addr addr, std::string path, NetworkTask* task){
    int sockfd = tcp_connect(addr, task->get_timeout());
    if(sockfd == -1){
        task->get_logger()->log_request(RequestStatus::FAIL,addr, "");
        close(sockfd);
        return RequestStatus::FAIL;
    }
    try{
        HttpRequestResult* result = http_get(sockfd, addr, path);
        if(!result->is_http()){
            task->get_logger()->log_request(RequestStatus::FAIL,addr, "");
            delete result;
            close(sockfd);
            return RequestStatus::FAIL;
        }
        task->push_raw_result(addr);
        std::vector<std::string> lines;
        lines.push_back("\e[1mserver:\e[0m "+result->get_header("Server"));
        lines.push_back("\e[1mcontent-type:\e[0m "+result->get_header("Content-Type"));
        lines.push_back("\e[1mcontent-length:\e[0m "+result->get_header("Content-Length"));
        if(result->get_header("Content-Type").find("text/html") != std::string::npos){
            std::stringstream stream("");
            size_t s;
            stream >> s;
            lines.push_back("\e[1mhtml-title:\e[0m "+html_get(result->get_content_ptr(), 0, result->get_content_size(), "html.head.title", 0));
        }
        task->get_logger()->log_request(RequestStatus::OK,addr, result->get_status()+" \e[33mhttp://"+addr.get_host()+":"+std::to_string(addr.port)+path, lines);
        delete result;
        close(sockfd);
        return RequestStatus::OK;
    }
    catch(NetworkError e){
        task->get_logger()->log_request(RequestStatus::FAIL,addr, e.what());
    }
    catch(DataException e){
        task->get_logger()->log_request(RequestStatus::OK,addr, e.what());
    }
    close(sockfd);
    return RequestStatus::FAIL;
}

bool HttpRequestResult::is_http(){
    if(m_size < 5) return false;
    if(m_raw_data[0] == 'H' && m_raw_data[1] == 'T' && m_raw_data[2] == 'T' && m_raw_data[3] == 'P'){
        return true;
    }
    return false;
}

std::string HttpRequestResult::get_status(){
    if(!is_http()) throw InvalidProtocolError("");
    char* i = strstr(m_raw_data, " ");
    if(i == NULL) return "";
    char* e = strstr(i+1, " ");
    if(e == NULL) return "";
    std::string result;
    result.append(i+1, e-i-1);
    return result;
}

std::string HttpRequestResult::get_full_header(){
    if(!is_http()) throw InvalidProtocolError("");
    char* i = strstr(m_raw_data, "\r\n\r\n");
    if(i == NULL) return "";
    std::string result;
    result.append(m_raw_data, i-m_raw_data);
    return result;
}

std::string HttpRequestResult::get_header(std::string key){
    if(!is_http()) throw InvalidProtocolError("");
    std::vector<std::string> lines = split_l(get_full_header(),"\r\n");
    for(int i=1; i<lines.size();i++){
        size_t pos = lines[i].find(key+":");
        if(pos == std::string::npos){
            continue;
        }
        size_t fpos = lines[i].find(": ")+2;
        if(fpos >= lines[i].size()){
            continue;
        }
        return lines[i].substr(fpos);
    }
    return "";
}

std::string HttpRequestResult::get_version(){
    if(!is_http()) throw InvalidProtocolError("");
    char* i = strstr(m_raw_data, " ");
    std::string result;
    result.append(m_raw_data, i-m_raw_data);
    return result;
}

char* HttpRequestResult::get_content_ptr(){
    if(!is_http()) throw InvalidProtocolError("");
    return strstr(m_raw_data, "\r\n\r\n")+4;
}

size_t HttpRequestResult::get_content_size(){
    return m_size - (get_content_ptr() - m_raw_data);
}