#include "Channel.hpp"

Channel::Channel(std::string& name) : mInviteOnly(false), mChannelName(name), mMaxUserNum(42) {}

Channel::~Channel() {}

void Channel::init(Client& channeloper) {
	mClientMap[channeloper.getNick()] = std::make_pair(channeloper, true);
}

void Channel::joinClient(Client& client) {
	mClientMap[client.getNick()] = std::make_pair(client, false);
}

int Channel::kickClient(std::string& caller, std::string& target) {
	if (isOper(caller) == false)
		return 1;			// Not an operator
	else if (mClientMap.find(target) == mClientMap.end())
		return 2;			// cannot find target
	else
		mClientMap.erase(target);
	return 0;				// sucessfully operated
}

bool Channel::isOper(std::string& clientname) {		// client always exists!
	return (mClientMap[clientname].second);
}
