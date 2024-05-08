#pragma once
# include <sys/socket.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <signal.h>
# include <unistd.h>
# include <fcntl.h>

# include <iostream>
# include <string>
# include <exception>
# include <map>
# include <vector>

// class Channel;
class EventHandler;
// class Client;

class Server
{
private:
	// std::map<std::string, Channel>	mChannelMap;
	int										mPort;
	std::string								mPassword;
	sockaddr_in 							mAddr;
	int		 								mSocket;
	struct kevent							mEventList;
	Server();
public:
	friend class EventHandler;
	Server(int argc, char **argv); //명령어 함수
	~Server();
	uintptr_t getSocket() const;
	// void	joinChannel(Client client, std::string channelName); //조인인데 없으면 분기해서 만든다
};
