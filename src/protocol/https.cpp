#include "https.h"
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

#include "../lapi.h"

HttpRequestResult* https_get(SSL* ssl, Ipv4Addr addr, std::string path){
    char* req_buffer = (char*)std::malloc(MIN_BUFFER_SIZE);
    std::snprintf(req_buffer, MIN_BUFFER_SIZE, "GET %s HTTP/1.1\r\n"\
    "Host: %s\r\n"\
    "Connection: Close\r\n"\
    "\r\n", path.c_str(), addr.to_string().c_str());

    size_t http_req_s = std::strlen(req_buffer);
    //char* req_buffer_ptr = req_buffer;

    size_t sent = 0;
    while(sent < http_req_s){
        int bytes = tls_send(ssl, req_buffer+sent,http_req_s-sent);
        if(bytes < 0){
            throw NetworkError("");
        }
        if(bytes == 0){
            break;
        }
        sent += bytes;
    };
    // tls_send(ssl, req_buffer,http_req_s);
    std::free(req_buffer);

    char* result_buffer = NULL;
    char* recv_buffer = new char[MIN_BUFFER_SIZE];
    std::memset(recv_buffer, 0, MIN_BUFFER_SIZE);
    size_t received = 0;
    while(1){
        int bytes = tls_recv(ssl,recv_buffer,MIN_BUFFER_SIZE-1);
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

int https_action(Ipv4Addr addr, std::string path, NetworkTask* task){
    tls_init();
    SSL* ssl = tls_connect(addr, task->get_timeout());
    if(ssl == NULL){
        task->get_logger()->log_request(RequestStatus::FAIL,addr, "");
        //tls_close(ssl);
        return RequestStatus::FAIL;
    }
    try{
        HttpRequestResult* result = https_get(ssl, addr, path);
        if(!result->is_http()){
            task->get_logger()->log_request(RequestStatus::FAIL,addr, "");
            delete result;
            tls_close(ssl);
            return RequestStatus::FAIL;
        }
        int callback = task->get_lua_protocol_callback("https");
        if(callback != 0){
            std::mutex lock;
            lock.lock();
            lua_State* L = task->get_lua_state();
            lua_rawgeti( L, LUA_REGISTRYINDEX, callback);
            l_push_ipv4addr(L, &addr);
            *reinterpret_cast<HttpRequestResult**>(lua_newuserdata(L, sizeof(HttpRequestResult*))) = result;
            luaL_setmetatable(L, HTTPREQUESTRESULT_STR);
            lua_pcall( L, 2, 0, 0 );
            lock.unlock();
        }
        http_log(addr, path, task, result);
        delete result;
        tls_close(ssl);
        return RequestStatus::OK;
    }
    catch(NetworkError e){
        task->get_logger()->log_request(RequestStatus::FAIL,addr, e.what());
    }
    catch(DataException e){
        task->get_logger()->log_request(RequestStatus::OK,addr, e.what());
    }
    tls_close(ssl);
    return RequestStatus::FAIL;
}