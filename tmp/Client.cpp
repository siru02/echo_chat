#include "Client.hpp"

Client::Client() {}

Client::Client(std::string& ip, int& socket, int& port) : mNick(ip), mUserName(ip), mIP(ip), mSocket(socket), mPort(port) {}

Client::~Client() {}

std::string	Client::getBuf()
{
	return mBuf;
}

void		Client::setBuf(std::string buf)
{
	mBuf = buf;
}

void		Client::clearBuf()
{
	mBuf.clear();
}

int			Client::getSocket()
{
	return mSocket;
}

std::string	Client::getIp()
{
	return mIP;
}
