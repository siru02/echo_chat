#include "EventHandler.hpp"
#include "Client.hpp"

EventHandler::EventHandler(Server& serv) {
	kq = kqueue();
	if (kq == -1)
		throw std::runtime_error("Error: cannot initialize kqueue");
}

EventHandler::~EventHandler() {}

void EventHandler::addNewEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
	struct kevent newEvent;

	EV_SET(&newEvent, ident, filter, flags, fflags, data, udata);
	//kevent구조체인 newEvent를 초기화해준다
	//ident는 연결해야하는 소켓fd이고 나머지는 kevent의 인자이다
	//함수 실행결과 kevent구조체에 이벤트가 등록된다
	changeLst.push_back(newEvent);
	//changeLst에 새로운 kevent를 추가한다
}

void EventHandler::handleEvents(uintptr_t serverSock) {
	newEvents = kevent(kq, &changeLst[0], changeLst.size(), eventLst, 8, NULL);
	//kevent는 kq의 모니터링할 이벤트를 새로 등록하고, 처리해야할 이벤트를 eventLst변수에 담아온다.
	// std::cout << "kevent: " << newEvents << ", changeLst.size(): " << changeLst.size() << std::endl;
	if (newEvents == -1)
		throw std::runtime_error("Error: kevent() failed");

	changeLst.clear();//추가하려고 하는 이벤트벡터는 루프돌기전에 초기화

	for (int i = 0; i < newEvents; i++) {
		//처리해야하는 이벤트들을 처리한다
		curEvent = &eventLst[i];
		//curEvent는 kevent구조체로 한 이벤트에 대한 정보를 담고있다 -> ident(fd), filter, flag, fflag등

		if (curEvent->flags & EV_ERROR) {//현재처리하는 이벤트가 오류이벤트이면	// 나중에 switch 문으로 바꾸기
			if (curEvent->ident == serverSock) //현재 이벤트가 서버소켓에 대한 이벤트오류이벤트이면
				throw std::runtime_error("Error occured in server socket");
			else { //클라이언트소켓에 대한 오류이벤트이면
				std::cerr << "client socket error\n";
				disconnectClient(curEvent->ident, clients);
			}
		}
		else if (curEvent->filter == EVFILT_READ) { //현재 이벤트가 READ해야하는 이벤트이면
			if (curEvent->ident == serverSock) //서버소켓에 의한 요청이면 새로운 클라이언트 접속요청임 //애매하게 이해함
				acceptNewClient(serverSock);
			else if (clients.find(curEvent->ident) != clients.end()) //클라이언트가 있으면
				readDataClient(); //소켓버퍼에서 정보를 읽어온다
		}
		else if (curEvent->filter == EVFILT_WRITE) {
			sendDataClient("default"); //write필터이면 kevent의 정보에 맞는 클라이언트에게 메시지를 보낸다
		}
	}
}

void EventHandler::acceptNewClient(int serverSock) {
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	memset(&clientAddr, 0, clientAddrLen);
	int clientSock = accept(serverSock, (sockaddr *)&clientAddr, &clientAddrLen);
	if (clientSock < 0)
		throw std::runtime_error("Error: client accept() failed");
	std::string clientIP = inet_ntoa(clientAddr.sin_addr);
	int clientPort = ntohs(clientAddr.sin_port);

	std::cout << "Accept new client\nIP: " << clientIP << ", port: " << clientPort << std::endl;
	fcntl(clientSock, F_SETFL, O_NONBLOCK);

	addNewEvent(clientSock, EVFILT_READ, EV_ADD |EV_ENABLE, 0, 0, NULL);
	addNewEvent(clientSock, EVFILT_WRITE, EV_ADD |EV_ENABLE, 0, 0, NULL);
	Client newClient(clientIP, clientSock, clientPort);
	clients[clientSock] = newClient;
	// mChannelMap["default"].addUser(clients[clientSock]);
}


void EventHandler::readDataClient() {
	char buffer[4242];
	int n = read(curEvent->ident, buffer, sizeof(buffer));

	if (n <= 0) {
		if (n < 0)
			std::cerr << "client read Error!" << std::endl;
		disconnectClient(curEvent->ident, clients);
	}
	else {
		std::cout << "READ\n";
		buffer[n] = '\0';
		clients[curEvent->ident].setBuf(clients[curEvent->ident].getBuf() += buffer);
		std::cout << buffer << '$' << std::endl;
	}
}

void EventHandler::sendDataClient(std::string channelName) {
	// std::map<std::string, Channel>::iterator it = mChannelMap.find(channelName);	// TODO, implement after channel setup
	// if (it != mChannelMap.end()) {
	// 	it->second.sendMsg(clients[curEvent->ident].getBuf());
	// }


	std::map<int, Client>::iterator it = clients.find(curEvent->ident);

	if (it != clients.end()) {
		if (clients[curEvent->ident].getBuf() != "") {
			for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
			{

				int n = write(it->second.getSocket(), clients[curEvent->ident].getBuf().c_str(), clients[curEvent->ident].getBuf().size());
				if (n < 0)
				{
					std::cerr << "Error on client write\n";
					disconnectClient(it->second.getSocket(), clients);
				}
			}
			clients[curEvent->ident].clearBuf();
		}
	}
}

void EventHandler::disconnectClient(int clientFd, std::map<int, Client>& clients) {
	std::cout << clients[clientFd].getIp() << " disconnected" << std::endl; // Fix the typo in the output message
	close(clientFd);
	clients.erase(clientFd);
}
