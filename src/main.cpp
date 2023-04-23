#include "networktask.h"
#include "logger.h"

const char cli_help_msg[] = "IniKiwi's netexplorer\n" \
"the manual is on https://github.com/IniKiwi/netexplorer\n";

#define TAG_EXPLORE "explore"
#define TAG_SEARCH "search"

uint32_t timeout = 500000;
uint32_t requests = 0;
uint32_t threads = 5;
bool hide_fail = false;

enum Tasks{
    TASK_EXPLORE,
    TASK_SEARCH
};

int task;
int task_argc;
std::string rawoutput_filename = "";
std::string log_filename = "";


int main(int argc, char *argv[]){
    Logger* logger = new Logger(true,"output.log");
    for(int a=1;a<argc;a++){
        if(std::string(argv[a]) == TAG_EXPLORE){
            if(argc < a+1){exit(1);}
            task == Tasks::TASK_EXPLORE;
            task_argc = a;
        }
        if(std::string(argv[a]) == TAG_SEARCH){
            if(argc < a+1){exit(1);}
            task == Tasks::TASK_SEARCH;
            task_argc = a;
        }
        if(std::string(argv[a]) == "-j"){
            if(argc < a+1){exit(1);}
            threads = atoi(argv[a+1]);
        }
        if(std::string(argv[a]) == "-t"){
            if(argc < a+1){exit(1);}
            timeout = atoi(argv[a+1]);
        }
        if(std::string(argv[a]) == "-r"){
            if(argc < a+1){exit(1);}
            requests = atoi(argv[a+1]);
        }
        if(std::string(argv[a]) == "--hide-fail"){
            hide_fail = true;
        }
         if(std::string(argv[a]) == "--raw-output"){
            if(argc < a+1){exit(1);}
            rawoutput_filename = argv[a+1];
        }
    }

    logger->set_hide_fail(hide_fail);
    if(task == Tasks::TASK_EXPLORE){
        NetworkTask* task = new NetworkTask(logger, argv[task_argc+1]);
        task->set_timeout(timeout);
        task->set_threads_num(threads);
        task->set_max_requests(requests);
        task->set_raw_output_filename(rawoutput_filename);
        task->run();
        task->write_raw_results();
    }

    delete logger;
}