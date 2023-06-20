#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

std::vector<std::string> split(const std::string &str, char delim);

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args );

std::vector<std::string> split_l(const std::string str, std::string dlm);
std::string string_replace_all(std::string str, const std::string& from, const std::string& to);