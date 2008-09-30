/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


/****************************************************************
	xerxes engine
	mac_network.cpp
 ****************************************************************/

/* network.cpp by AegisKnight, aka Chad Austin */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include "xerxes.h"

#ifdef __LINUX__
std::string getUrlText(std::string url)
{
	err("getUrlText is TODO in Linux");
}

int getUrlImage(std::string url)
{
	err("getUrlImage is TODO in Linux");
}
#endif

static void InitializeNetwork()
{
}


////////////////////////////////////////////////////////////////////////////////
// NetworkException ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

NetworkException::NetworkException(const char* message)
: m_message(message)
{
}

NetworkException::~NetworkException() throw ()
{
}


////////////////////////////////////////////////////////////////////////////////

const char*
NetworkException::what() const throw()
{
    return m_message.c_str();
}

////////////////////////////////////////////////////////////////////////////////
// ServerSocket ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ServerSocket::ServerSocket(int port)
{
    InitializeNetwork();

    // create the socket
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == 0) {
        throw NetworkException("socket() failed");
    }

    // make it nonblocking
    unsigned long ul = 1;
    if (ioctl(m_socket, FIONBIO, &ul) != 0) {
        close(m_socket);
        throw NetworkException("ioctlsocket() failed");
    }

    // bind the socket to an address
    sockaddr_in name;
    memset(&name, 0, sizeof(name));
    name.sin_family      = AF_INET;
    name.sin_addr.s_addr = INADDR_ANY;
    name.sin_port        = htons(port);
    if (bind(m_socket, (sockaddr*)&name, sizeof(name)) != 0) {
        close(m_socket);
        throw NetworkException("bind() failed");
    }

    // now listen on it
    if (listen(m_socket, 1) != 0) {
        close(m_socket);
        throw NetworkException("listen() failed");
    }
}

////////////////////////////////////////////////////////////////////////////////

ServerSocket::~ServerSocket()
{
    close(m_socket);
}

////////////////////////////////////////////////////////////////////////////////

Socket*
ServerSocket::accept()
{
    if (m_socket == 0) {
        return NULL;
    }

    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int s = ::accept(m_socket, (sockaddr*)&addr, &addrlen);
    if (s == -1) {
        return NULL;
    }

    return new Socket(s);
}

////////////////////////////////////////////////////////////////////////////////
// Socket //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Socket::Socket(const char* address, int port)
{
    InitializeNetwork();

    // resolve address
    hostent* hostptr = gethostbyname(address);
    if (hostptr == NULL) {
        throw NetworkException("gethostbyname() failed");
    }

    // create socket
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == 0) {
        throw NetworkException("socket() failed");
    }

    // connect
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr   = *(in_addr*)(hostptr->h_addr);
    addr.sin_port   = htons(port);
    if (connect(m_socket, (sockaddr*)&addr, sizeof(addr)) != 0) {
        close(m_socket);
        throw NetworkException("connect() failed");
    }

	// set nonblocking
	u_long argp = 1L;
	ioctl(m_socket, FIONBIO, &argp);

	// set tcp_nodelay
	char argc = 1;
	setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, &argc, 1);
}

////////////////////////////////////////////////////////////////////////////////

Socket::~Socket()
{
    close(m_socket);
}

////////////////////////////////////////////////////////////////////////////////

int
Socket::write(int size, const void* bytes)
{
    return send(m_socket, (char*)bytes, size, 0);
}

////////////////////////////////////////////////////////////////////////////////

int
Socket::read(int size, void* bytes)
{
    return recv(m_socket, (char*)bytes, size, 0);
}

////////////////////////////////////////////////////////////////////////////////

#ifndef SOCKET_ERROR
#define	SOCKET_ERROR	-1
#endif

int
Socket::nonblockread(int size, void* bytes)
{
	int flags = MSG_DONTWAIT;
	int recvd = 0;

	// Receives a nonblocked packet.
	recvd = recv (m_socket, (char*) bytes, size, flags);

	return (recvd == SOCKET_ERROR) ? 0 : recvd;
}

////////////////////////////////////////////////////////////////////////////////

int
Socket::blockread(int size, void* bytes)
{
	int recvd = 0;
	char *buf = (char *) bytes;
	while (true)
	{
		int ret = recv(m_socket, &buf[recvd], size-recvd, 0);
		if (ret>0)
		{
			recvd += ret;
			if (recvd == size)
				return recvd;
		}
		if (ret == 0)
		{
			// socket disconnected
			return 0;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

int
Socket::dataready()
{
    if(this->byteCount()) return 1;
    return 0;
}

int 
Socket::byteCount() 
{
	static char buf[2];
	int ret = recv(m_socket, buf, 1, MSG_PEEK);
    return ret;
} 


////////////////////////////////////////////////////////////////////////////////

int
Socket::connected()
{
    static char buf[2];
	int ret = recv(m_socket, buf, 1, MSG_PEEK);
	if (ret == 0) return 0;
	return 1;
}

////////////////////////////////////////////////////////////////////////////////

Socket::Socket(int socket)
: m_socket(socket)
{
}

////////////////////////////////////////////////////////////////////////////////

