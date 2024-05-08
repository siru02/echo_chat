#pragma once
#include "Server.hpp"
#include <algorithm>

class Client;

class EventHandler
{
private:
	// std::map<std::string, Channel>&	mChannelMap;	// TODO
	std::map<int, Client> clients; //소켓fd와 클라이언트 객체
	std::vector<struct kevent> changeLst; //kqueue에 추가하려고 하는 검사대상 이벤트 리스트
	struct kevent eventLst[8]; //kevent함수 실행시 발생한 이벤트들이 저장되는 리스트
	int kq;
	int newEvents;
	struct kevent* curEvent;
public:
	EventHandler(Server& serv);
	~EventHandler();
	void addNewEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
	void handleEvents(uintptr_t serverSock);

	void acceptNewClient(int serverSock);
	void readDataClient();
	void sendDataClient(std::string channelName);

	void disconnectClient(int clientFd, std::map<int, Client>& clients);
};
