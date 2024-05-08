#include "Server.hpp"
#include "Channel.hpp"

Server::Server(int argc, char **argv) {
	if (argc < 2 || argc > 3)
		throw std::runtime_error("Usage: <server> <port number> <password(optional)>");

	char *endptr; //포트번호입력에 숫자이외의 문자가 있다면 여기에 저장된다
	mPort = strtol(argv[1], &endptr, 10); //10진수로 정의된 문자열을 정수로 변환한다
	if (argc == 3) {
		mPassword = argv[2];
	}
	std::string endptrStr(endptr);
	if (mPort < 0 || mPort > 65535 || endptrStr.size()) //port범위안에 + 숫자만 입력되는 조건으로
		throw std::runtime_error("ERROR: invalid mPort number");
	//port번호 유효한 범위인지 확인

	//socket creation
	mSocket = socket(AF_INET, SOCK_STREAM, 0); //소켓열기
	if (mSocket == -1)
		throw std::runtime_error("ERROR: socket() failed");
	memset(&mAddr, 0, sizeof(mAddr));
	mAddr.sin_family = AF_INET;
	mAddr.sin_port = htons(mPort);

	//binding
	if (bind(mSocket, (struct sockaddr*)&mAddr, sizeof(mAddr)) == -1)
		throw std::runtime_error("ERROR: bind() failed");

	//listening
	if (listen(mSocket, INT_MAX) == -1)
		throw std::runtime_error("ERROR: listen() failed");
	fcntl(mSocket, F_SETFL, O_NONBLOCK);

	// Channel defaultCh("default");
	// mChannelMap["default"] = defaultCh;
}

Server::~Server() {}

uintptr_t Server::getSocket() const {
	return mSocket;
}
