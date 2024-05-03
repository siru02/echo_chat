#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char *argv[])
{
 	if (argc < 2){ //인자개수 오류의 경우를 처리한다
        std::cout << "Usage: " << argv[0] << " <port number>" << std::endl;
        return 0;
    }
    int port = atoi(argv[1]); //1번째 인자로 포트번호를 입력받는다

	//socket creation
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	//AF_INET은 IPv4에서 통신함을 의미, SOCK_STREAM은 TCP방식의 소켓임을 의미
	//소켓이 만들어지고 그 소켓에 대한 fd값이 반환된다
	if (sock == -1){
		perror("socket create error\n");
		//std::cout << "socket create failed" << std::endl;
		return 1;
	}

	//socket binding
	//struct sockaddr listeningSock; //소켓의 ip와 port를 저장하는 구조체
	struct sockaddr_in listeningSock; //소켓의 ip와 port를 저장하는 구조체
	//리눅스,유닉스 시스템에서는 소켓의 통신 대상을 지정하기 위해 주소를 사용한다
	//sockaddr은 일반적인 소켓주소를 담는 구조체이고, sockaddr_in은 IPv4(AF_INET)의 정보를 담기에 최적화된 구조체이다.
	memset(&listeningSock, 0, sizeof(listeningSock)); //소켓구조체의 내부값은 0으로 전부 초기화시키고 시작한다
	if (bind(sock, (struct sockaddr *)&listeningSock, sizeof(listeningSock)) == -1) //sockaddr_in은 sockaddr형태로 캐스팅
	{
		perror("bind error\n");
		return 1;
	}

	//socket listening
	if (listen(sock, 1) < 0)//2번째 인자는 몇개의 클라이언트로부터 연결가능한지를 의미하는데 임의로 10명이라고 설정해봄
	{
		perror("listening error\n");
		return 1;
	}
	std::cout << "socket listening" << std::endl;
	//연결중에는 대기상태이니(blocking) 연결완료시 실행

	//accept
	//struct sockaddr remote_address;
	struct sockaddr_in remote_address; //IPv4주소체계의 소켓을 저장하는 구조체
	memset(&remote_address, 0, sizeof(remote_address));
	socklen_t remote_addr_len = sizeof(remote_address); //소켓구조체의 길이나 크기를 저장한다
	std::cout << "wait" << std::endl;
	int client_sock = accept(sock, (struct sockaddr *)&remote_address, &remote_addr_len);
	//tcp 연결로 리스팅소켓에서 커넥션이 만들어지면 클라이언트소켓을 구분하는 client_sock의 fd를 저장하고 client소켓의 정보를 remote_address 구조체에 저장
	if (client_sock < 0){
		perror("accept fail\n");
		return 1;
	}

	std::string client_ip = inet_ntoa(remote_address.sin_addr);//클라이언트의 sockaddr_in구조체에서 IPv4주소를 가져와
	//inet_ntoa에는 인자로 addr_in구조체가 와야하며 이는 호스트IP주소를 담고있다
	//inet_ntoa함수는 네트워크 바이트 순서의 32비트 값을 Dotted-Decimal Notation으로 변환
	//inet_aton()함수와 반대로 동작한다
	int remote_port = ntohs(remote_address.sin_port); //포트번호를 Intel프로세서의 little-endian으로 변환
	 std::cout << "Accepted new client @ " << client_ip << ":" << remote_port << std::endl; //연결된 정보 출력

	int BUFFLEN = 1024;
	char buffer[BUFFLEN];
	std::cout << "Accept success" << std::endl;

	while (1){
		memset(buffer, 0, BUFFLEN);
		// recv
		int bytes_received = recv(client_sock, buffer, BUFFLEN -1 ,0);
		//클라이언트 소켕으로부터 받은 값을 버퍼에 넣는다.
		if (bytes_received < 0){
			perror ("recv fail\n");
			return 1;
		}
		if (bytes_received == 0){
			std::cout << "Client at " << client_ip << ":" << remote_port << " has disconnected." << std::endl;
            break;
		}
		if (buffer[bytes_received-1] == '\n'){
            buffer[bytes_received-1] = 0;
        }

		//send
		 std::string response = "Hello client at " + client_ip + ":" + std::to_string(remote_port) + ". Your message was \n\"" + std::string(buffer) + "\"\n";

        int bytes_sent = send(client_sock, response.c_str(), response.length(), 0);//sizeof string이 아닌 길이를 반환하도록 해야한다
        if (bytes_sent < 0){
            perror("Could not send");
            return 1;
        }
	}
	std::cout << "Shutting down socket." << std::endl;
    shutdown(client_sock, SHUT_RDWR);
}