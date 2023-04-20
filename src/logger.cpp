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

void Logger::log(std::string msg){
    m_lock.lock();
    if(m_use_stdout){
        printf("\e[90m%s -\e[39m %s\e[0m\n", get_time_str().c_str(), msg.c_str());
    }
    if(m_logfile.is_open()){
        m_logfile << get_time_str();
        m_logfile << " \t";
        m_logfile << msg;
        m_logfile << "\n";
    }
    m_lock.unlock();
}

void Logger::log_request(int status, Ipv4Addr addr, std::string msg){
    if(status == RequestStatus::FAIL && m_hide_fail == true) return;
    char* buffer = new char[1000+msg.size()];
    const char* st;
    if(status == RequestStatus::OK) st = MSG_OK;
    if(status == RequestStatus::FAIL) st = MSG_FAIL;
    if(status == RequestStatus::SKIPPED) st = MSG_SKIPPED;
    sprintf(buffer,"try connect to %s:%d -> %s %s", addr.to_string_ip().c_str(),addr.port, st, msg.c_str());
    log(buffer);
    delete buffer;
}