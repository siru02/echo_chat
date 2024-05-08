#pragma once
#include "Server.hpp"
#include "Client.hpp"

class Server;
class Client;

class Channel
{
private:
	std::map<std::string, std::pair<Client&, bool> >	mClientMap; // <username, <class Client, operator auth>>
	bool		mInviteOnly;
	bool		mTopicChangeAuth;
	std::string	mChannelName;
	std::string	mTopic;
	std::string mChannelPw;
	int			mMaxUserNum;
	bool		isOper(std::string& clientname);

public:
	Channel(std::string& name);
	~Channel();
	void init(Client& channeloper);
	void joinClient(Client& client);
	int kickClient(std::string& caller, std::string& target);
	// void setClientMode(std::string& clientname);		// MODE -o
};
