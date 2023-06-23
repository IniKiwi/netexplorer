#pragma once
#include <exception>
#include <string>

class BaseException : public std::exception{
    protected:
    std::string message;
    public:
    BaseException(const std::string &msg): message(msg) {};
	~BaseException() = default;

    virtual const char* what(){
		return message.c_str();
	}
};

class NetworkError : public BaseException {
    public:
	NetworkError(const std::string &s): BaseException(s) {}
};

class MemoryError : public BaseException {
    public:
	MemoryError(const std::string &s): BaseException(s) {}
};

class InvalidProtocolError : public BaseException {
    public:
	InvalidProtocolError(const std::string &s): BaseException(s) {}
};

class DataException : public BaseException {
    public:
	DataException(const std::string &s): BaseException(s) {}
};

class FileNotFoundException : public BaseException {
    public:
	FileNotFoundException(const std::string &s): BaseException(s) {}
};