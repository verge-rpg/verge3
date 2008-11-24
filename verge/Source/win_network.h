/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef NETWORK_HPP
#define NETWORK_HPP


#include <winsock2.h>
#include <wininet.h>
#include <string>
#include <exception>

#include "a_string.h"

StringRef getUrlText(CStringRef url);
int getUrlImage(CStringRef url);

class NetworkException : public std::exception
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
    SOCKET m_socket;
};


class Socket
{
    friend ServerSocket;

public:
    Socket(const char* address, int port);
    ~Socket();

    int write(int size, const void* bytes);
	int read(int size, void* bytes);
	int nonblockread(int size, void* bytes); // Overkill (2008-04-28): A nonblocking read operation.
	int blockread(int size, void* bytes);
	int dataready();
	int connected();
	int byteCount(); // Overkill (2008-04-20): Peek at how many bytes are in buffer. Requested by ustor.

private:
    Socket(SOCKET socket);

private:
    SOCKET m_socket;
};


#endif
