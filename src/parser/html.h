#pragma once

#include <string>
size_t html_next_token_pos(const char* data, size_t pos, size_t size);
std::string html_next_token(const char* data, size_t pos, size_t size);
size_t html_next_token_size(const char* data, size_t pos, size_t size);
size_t html_next_token_name_size(const char* data, size_t pos, size_t size);

std::string html_get(const char* data, size_t pos, size_t size, std::string name, size_t index);

std::string html_token_get(const char* data, std::string token, size_t pos);