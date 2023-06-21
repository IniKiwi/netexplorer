#pragma once
#include "networktask.h"
#include <string>
#include <fstream>
#include <mutex>

#define COLOR_RED "\e[31m"
#define COLOR_GREEN "\e[32m"
#define COLOR_BLUE "\e[34m"

//bolt - green - OK! - normal color - normal text
#define MSG_OK "\e[1m\e[32mOK\e[39m\e[0m"
#define MSG_FAIL "\e[1m\e[31mFAIL\e[39m\e[0m"
#define MSG_SKIPPED "\e[1m\e[33mSKIPPED\e[39m\e[0m"
#define MSG_ACCESS_DENIED "\e[1m" COLOR_RED "ACCESS DENIED\e[39m\e[0m"
#define MSG_ERROR "\e[1m\e[31m"

enum RequestStatus{
    OK,
    FAIL,
    SKIPPED,
    ACCESS_DENIED
};

class Logger{
    private:
    std::mutex m_lock;
    bool m_use_stdout;
    bool m_hide_fail;
    bool m_hide_access_denied;
    std::ofstream m_logfile;

    public:
    Logger(bool _use_stdout, std::string output): m_use_stdout(_use_stdout) {
        m_logfile.open(output, std::ios::out | std::ios::app);
    }
    ~Logger(){
        if(m_logfile.is_open()){
            m_logfile.close();
        }
    }

    std::string get_time_str();
    void set_hide_fail(bool v){m_hide_fail = v;}
    void set_hide_access_denied(bool v){m_hide_access_denied = v;}
    void log(std::string msg);
    void log_unsafe(std::string msg);
    void log_request(int status, Ipv4Addr addr, std::string msg);
    void log_request(int status, Ipv4Addr addr, std::string msg, std::vector<std::string> lines);
};