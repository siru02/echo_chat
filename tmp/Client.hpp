#pragma once
#include "Server.hpp"

class Server;
class Channel;

class Client
{
private:
	std::string mNick;
	std::string	mUserName;
	std::string	mIP;
	int			mSocket;
	int			mPort;
	std::string mBuf;

public:
	Client();
	Client(std::string& ip, int& socket, int& port);
	~Client();
	std::string getBuf();
	void setBuf(std::string buf);
	void clearBuf();
	int getSocket();
	std::string& getIp();
	std::string& getNick();
};
