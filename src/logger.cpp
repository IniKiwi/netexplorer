#include "logger.h"
#include "util.h"

#include <ctime>
#include <cstring>
#include <cstdio>

std::string Logger::get_time_str(){
    time_t now = std::time(0);
    tm* ltm = std::localtime(&now);

    char* buffer = new char[1000];

    sprintf(buffer,"%04d/%02d/%02d %02d:%02d:%02d",
        1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday, 
        ltm->tm_hour, ltm->tm_min, ltm->tm_sec);

    std::string ret = std::string(buffer);
    delete buffer;
    return  ret;
}

void Logger::log_unsafe(std::string msg){
    if(m_use_stdout){
        printf("\e[90m%s -\e[39m %s\e[0m\n", get_time_str().c_str(), msg.c_str());
    }
    if(m_logfile.is_open()){
        m_logfile << get_time_str();
        m_logfile << " \t";
        m_logfile << msg;
        m_logfile << "\n";
    }
}

void Logger::log(std::string msg){
    m_lock.lock();
    log_unsafe(msg);
    m_lock.unlock();
}

void Logger::log_request(int status, Ipv4Addr addr, std::string msg){
    log_request(status, addr, msg, std::vector<std::string>());
}

void Logger::log_request(int status, Ipv4Addr addr, std::string msg, std::vector<std::string> lines){
    if(status == RequestStatus::FAIL && m_hide_fail == true) return;
    char* buffer = new char[1000+msg.size()];
    const char* st;
    if(status == RequestStatus::OK) st = MSG_OK;
    if(status == RequestStatus::FAIL) st = MSG_FAIL;
    if(status == RequestStatus::SKIPPED) st = MSG_SKIPPED;
    if(status == RequestStatus::ACCESS_DENIED) st = MSG_ACCESS_DENIED;
    sprintf(buffer,"try connect to %s://%s:%d -> %s %s", addr.protocol.c_str(), addr.to_string_ip().c_str(),addr.port, st, msg.c_str());
    m_lock.lock();
    log_unsafe(buffer);
    delete buffer;
    for(int i=0;i<lines.size();i++){
        size_t pos = lines[i].find(':');
        if(pos != std::string::npos){
            lines[i].insert(pos+1, "\e[0m");
            lines[i].insert(0, "\e[1m");
        }
        lines[i] = string_replace_all(lines[i], "\n", "\n\t\t\t");
        log_unsafe("\t"+lines[i]);
    }
    m_lock.unlock();
}