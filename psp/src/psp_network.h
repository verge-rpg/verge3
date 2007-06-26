#ifndef NETWORK_HPP
#define NETWORK_HPP


//#include <winsock2.h>
//#include <wininet.h>
#include <string>
#include <exception>

#include "a_string.h"

string getUrlText(string url);
int getUrlImage(string url);

class NetworkException //: public std::exception
{
public:
    NetworkException(const char* message);
    virtual const char* what() const throw();

private:
    std::string m_message;
};


class Socket;  // forward declaration


class ServerSocket
{
public:
    ServerSocket(int port);
    ~ServerSocket();

    Socket* accept();

private:
    //SOCKET m_socket;
};


class Socket
{
    friend class ServerSocket;

public:
    Socket(const char* address, int port);
    ~Socket();

    int write(int size, const void* bytes);
	int read(int size, void* bytes);
	int blockread(int size, void* bytes);
	int dataready();
	int connected();

private:
    //Socket(SOCKET socket);

private:
    //SOCKET m_socket;
};


#endif
