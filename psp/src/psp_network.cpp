/****************************************************************
	xerxes engine
	psp_network.cpp
 ****************************************************************/

/* network.cpp by AegisKnight, aka Chad Austin */
#include "xerxes.h"
#include "psp_network.h"

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
    //m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //if (m_socket == INVALID_SOCKET) {
    //    throw NetworkException("socket() failed");
    //}

    //// make it nonblocking
    //unsigned long ul = 1;
    //if (ioctlsocket(m_socket, FIONBIO, &ul) != 0) {
    //    closesocket(m_socket);
    //    throw NetworkException("ioctlsocket() failed");
    //}

    //// bind the socket to an address
    //sockaddr_in name;
    //memset(&name, 0, sizeof(name));
    //name.sin_family      = AF_INET;
    //name.sin_addr.s_addr = INADDR_ANY;
    //name.sin_port        = htons(port);
    //if (bind(m_socket, (sockaddr*)&name, sizeof(name)) != 0) {
    //    closesocket(m_socket);
    //    throw NetworkException("bind() failed");
    //}

    //// now listen on it
    //if (listen(m_socket, 1) != 0) {
    //    closesocket(m_socket);
    //    throw NetworkException("listen() failed");
    //}
}

////////////////////////////////////////////////////////////////////////////////

ServerSocket::~ServerSocket()
{
  //  closesocket(m_socket);
}

////////////////////////////////////////////////////////////////////////////////

Socket*
ServerSocket::accept()
{
    //if (m_socket == INVALID_SOCKET) {
    //    return NULL;
    //}

    //sockaddr_in addr;
    //int addrlen = sizeof(addr);
    //SOCKET s = ::accept(m_socket, (sockaddr*)&addr, &addrlen);
    //if (s == INVALID_SOCKET) {
    //    return NULL;
    //}

    //return new Socket(s);
	throw NetworkException("accept() unimplemented!");
}

////////////////////////////////////////////////////////////////////////////////
// Socket //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Socket::Socket(const char* address, int port)
{
 //   InitializeNetwork();
	//
 //   // resolve address
 //   hostent* hostptr = gethostbyname(address);
 //   if (hostptr == NULL) {
 //       throw NetworkException("gethostbyname() failed");
 //   }

 //   // create socket
 //   m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
 //   if (m_socket == INVALID_SOCKET) {
 //       throw NetworkException("socket() failed");
 //   }

 //   // connect
 //   sockaddr_in addr;
 //   memset(&addr, 0, sizeof(addr));
 //   addr.sin_family = AF_INET;
 //   addr.sin_addr   = *(in_addr*)(hostptr->h_addr);
 //   addr.sin_port   = htons(port);
 //   if (connect(m_socket, (sockaddr*)&addr, sizeof(addr)) != 0) {
 //       closesocket(m_socket);
 //       throw NetworkException("connect() failed");
 //   }

	//// set nonblocking
	//u_long argp = 1L;
	//int result = ioctlsocket(m_socket, FIONBIO, &argp);

	//// set tcp_nodelay
	//char argc = 1;
	//setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, &argc, 1);
}

////////////////////////////////////////////////////////////////////////////////

Socket::~Socket()
{
    //closesocket(m_socket);
}

////////////////////////////////////////////////////////////////////////////////

int
Socket::write(int size, const void* bytes)
{
    //return send(m_socket, (char*)bytes, size, 0);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

int
Socket::read(int size, void* bytes)
{
    //return recv(m_socket, (char*)bytes, size, 0);
	return -1;
}

////////////////////////////////////////////////////////////////////////////////

int
Socket::blockread(int size, void* bytes)
{
	//int recvd = 0;
	//char *buf = (char *) bytes;
	//while (true)
	//{
	//	int ret = recv(m_socket, &buf[recvd], size-recvd, 0);
	//	if (ret>0)
	//	{
	//		recvd += ret;
	//		if (recvd == size)
	//			return recvd;
	//	}
	//	if (ret == 0)
	//	{
	//		// socket disconnected 
	//		return 0;
	//	}
	//}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

int 
Socket::dataready()
{
	//static char buf[2];
	//int ret = recv(m_socket, buf, 1, MSG_PEEK);	
	//if (ret	> 0) return 1;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

int 
Socket::connected()
{
 //   static char buf[2];
	//int ret = recv(m_socket, buf, 1, MSG_PEEK);
	//if (ret == 0) return 0;
	//return 1;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

//Socket::Socket(SOCKET socket)
//: m_socket(socket)
//{
//}

////////////////////////////////////////////////////////////////////////////////

string getUrlText(string url) 
{
	//HINTERNET h = InternetOpen("Verge 3.0", INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
	//if (!h)
	//{
	//	return "could not establish an internet connection.";
	//}
	//
	//HINTERNET hurl = InternetOpenUrl(h, url.c_str(), 0, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
	//if (!hurl)
	//{
	//	InternetCloseHandle(h);
	//	return "couldn't open url";
	//}
	//
	//char buf[1024];
	//string toReturn = "";
	//
	//BOOL b = true;
	//DWORD bytesread;
	//while (b)
	//{
	//	b = InternetReadFile(hurl,buf,1023,&bytesread);
	//	if (b && bytesread == 0)
	//		break;
	//	buf[bytesread] = 0;
	//	if (b) toReturn = toReturn + buf;
	//}
	//
	//InternetCloseHandle(h);
	//InternetCloseHandle(hurl);
	//
	//return toReturn;
	return string("");
}

int getUrlImage(string url)
{
	//int toReturn = 0;
	//HINTERNET h = InternetOpen("Verge 3.0", INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
	//if (!h)
	//{
	//	return 0;
	//}
	//
	//HINTERNET hurl = InternetOpenUrl(h, url.c_str(), 0, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
	//if (!hurl)
	//{
	//	InternetCloseHandle(h);
	//	return 0;
	//}
	//
	//char buf[1024];
	//FILE *f = fopen("$$urlimagetemp.$$$","wb");
	//
	//BOOL b = true;
	//DWORD bytesread;
	//while (b)
	//{
	//	b = InternetReadFile(hurl,buf,1023,&bytesread);
	//	if (b && bytesread == 0)
	//		break;
	//	fwrite(buf, 1, bytesread, f);
	//}
	//fclose(f);
	//
	//InternetCloseHandle(h);
	//InternetCloseHandle(hurl);
	//
	//toReturn = HandleForImage(xLoadImage("$$urlimagetemp.$$$"));
	//remove("$$urlimagetemp.$$$");
	//
	//return toReturn;
	return -1;
}
