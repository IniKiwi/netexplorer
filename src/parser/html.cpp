#include "html.h"
#include "../util.h"

#include <cstring>
#include <algorithm>

size_t html_next_token_pos(const char* data, size_t pos, size_t size){
    size_t i = pos;
    while(1){
        if(data[i] == 0x00 || i == size){
            return std::string::npos;
        }
        if(data[i] == '<'){
            if(data[i+1] == ' '){
                continue;
            }
            else{
                return i;
            }
        }
        i++;
    }
    return 0;
}

std::string html_next_token(const char* data, size_t pos, size_t size){
    std::string result;
    size_t i = html_next_token_pos(data, pos, size);
    size_t e = html_next_token_name_size(data, pos, size);
    
    
    result.append(data, i+1, e);
    std::transform(result.begin(), result.end(), result.begin(),
    [](unsigned char c){ return std::tolower(c); });
    return result;
}

size_t html_next_token_size(const char* data, size_t pos, size_t size){
    size_t i = html_next_token_pos(data, pos, size);
    size_t e = i;
    
    while(1){
        if(data[e] == 0x00 || e == size){
            return std::string::npos;
        }
        if(data[e] == '>'){
            break;
        }
        e++;
    }
    return e - i;
}

size_t html_next_token_name_size(const char* data, size_t pos, size_t size){
    size_t i = html_next_token_pos(data, pos, size);
    size_t e = i;
    
    while(1){
        if(data[e] == 0x00 || e == size){
            return std::string::npos;
        }
        if(data[e] == '>' || data[e] == ' '){
            break;
        }
        e++;
    }
    return e - i-1;
}

std::string html_get(const char* data, size_t pos, size_t size, std::string name, size_t index){
    std::vector<std::string> path = split(name, '.');
    std::vector<std::string> stack;
    index++;
    size_t i = pos;
    size_t found = 0;
    size_t start;
    size_t end;
    while(1){
        i = html_next_token_pos(data, i, size);
        if(i == std::string::npos){return "";}
        std::string token = html_next_token(data, i, size);
        if(token == ""){return "";}
        else if(token[0] == '!'){}
        else if(token[0] == '/'){
            if(path[path.size()-1] == token.substr(1)){
                if(path == stack){
                    if(found == index){
                        end = i;
                        return std::string(data, start, end - start);
                    }
                }
            }
            for(size_t s = stack.size()-1; s>=0;s--){
                if(stack.size() == 0){break;}
                if(stack[s] == token.substr(1)){
                    for(size_t c=0;c<stack.size()-s;c++){
                        if(stack.size()>0){
                            stack.pop_back();
                        }
                    }
                    break;
                }
            }
        }
        else{
            bool ok = true;
            if(token.compare("meta") == 0){ok = false;}
            if(token.compare("br") == 0){ok = false;}
            if(token.compare("link") == 0){ok = false;}
            if(token.compare("doctype") == 0){ok = false;}
            if(ok == true){
                stack.push_back(token);
                if(path == stack){
                    start = i+html_next_token_size(data,i,size)+1;
                    found++;
                }
            }
        }

        size_t next = html_next_token_size(data, i, size);
        if(next == std::string::npos){return "";}
        i += next-1;
    }
    
}