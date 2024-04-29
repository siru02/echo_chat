#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2){ //인자개수 오류의 경우를 처리한다
        std::cout << "Usage: " << argv[0] << " <port number>" << std::endl;
        return 0;
    }
    int port = atoi(argv[1]); //1번째 인자로 포트번호를 입력받는다

    // 1.socket
    //소켓함수는 sys/socket.h에 포함되어있다
    int sock = socket(AF_INET, SOCK_STREAM, 0); //소켓함수의 인자는 3개이며 순서대로 도메인, 타입, 프로토콜이다
    //일반적으로 인터넷에 연결하는 소켓은 AF_INET이며 PF_INET과 동일하다
    //tcp소켓이므로 sock_stream

    // 2. bind
    struct sockaddr_in address; //SOCKADDR_IN 구조체는 AF_INET 주소 패밀리에 대한 전송 주소 및 포트를 지정합니다.
    memset(&address, 0, sizeof(address)); //본인의 주소를 0으로 설정하는 파트가 있어야한다?
    address.sin_family = AF_INET;
    address.sin_port = htons(port); // = port로 저장해도 되지만 몇몇 cpu는 저장순서가 반대이기 때문에 함수를 사용하여 저장 -> 리틀엔디안 빅엔디안 이슈

    int bind_value = bind(sock, (struct sockaddr *)&address, sizeof(address));
    //바인드실패시 음수반환하므로 에러처리
    if (bind_value < 0){
        perror("Could not bind");
        return 1;
    }
    std::cout << "Bind success" << std::endl;

    // 3. listen
    int listen_value = listen(sock, 1);
    if (listen_value < 0){
        perror("Could not listen");
        return 1;
    }
    std::cout << "Listen success" << std::endl;

    // 4. accept
    struct sockaddr_in remote_address;
    memset(&remote_address, 0, sizeof(remote_address)); //원격주소에도 설정해줘야한다?
    socklen_t remote_addrlen = sizeof(remote_address);

    std::cout << "Waiting for new connection" << std::endl;

    int client_socket = accept(sock, (struct sockaddr *)&remote_address, &remote_addrlen); //소켓에서 accept를 호출하여 원격 주소를 캐스팅하여 저장
    //클라이언트의 정보 저장하고 액셉트함수는 새 연결이 생길 때까지 대기한다
    if (client_socket < 0){
        perror("Could not accept");
        return 1;
    }

    std::string client_ip = inet_ntoa(remote_address.sin_addr);//(Ipv4) 인터넷 네트워크 주소를 인터넷 표준 점선 소수점 형식의 ASCII 문자열로 변환
    int remote_port = ntohs(remote_address.sin_port);//htons를 뒤집은 형태이며 네트워크 순서 바이트를 호스트 바이트 순서(Intel 프로세서의 little-endian)로 변환

    std::cout << "Accepted new client @ " << client_ip << ":" << remote_port << std::endl; //연결된 정보 출력

    int BUFFLEN = 1024;
    char buffer[BUFFLEN];
    std::cout << "Accept success" << std::endl;

    while (1){
        memset(buffer, 0, BUFFLEN); //버퍼를 항상 null로 종료해야한다 -> 수신시마다 버퍼를 0으로 초기화한다
        // 5. recv
        //이제 버퍼에는 클라이언트로부터 받은 정보들이 비트단위로 저장되고 있다
        int bytes_received = recv(client_socket, buffer, BUFFLEN - 1, 0); //BUFFLEN -1 까지만 값을 받는다는게 중요하다 -> 만약 버퍼사이즈만큼 입력받으면 문자열길이 함수가 제대로 동작하지 못함
        //받은 바이트수는 굉장히 중요하다 -> 음수이면 에러가 발생, 0이면 연결이 끊어짐, 양수이면 무언가를 받았다는 의미
        if (bytes_received < 0){
            perror("Could not receive");
            return 1;
        }
        if (bytes_received == 0){
            std::cout << "Client at " << client_ip << ":" << remote_port << " has disconnected." << std::endl;
            break;
        }

        if (buffer[bytes_received-1] == '\n'){
            buffer[bytes_received-1] = 0;
        }
        // 6. send
        std::string response = "Hello client at " + client_ip + ":" + std::to_string(remote_port) + ". Your message was \n\"" + std::string(buffer) + "\"\n";

        int bytes_sent = send(client_socket, response.c_str(), response.length(), 0);//sizeof string이 아닌 길이를 반환하도록 해야한다
        if (bytes_sent < 0){
            perror("Could not send");
            return 1;
        }
    
    }

    //반복문을 탈출했다면 연결이 끊어진것이므로 소켓을 닫아야한다
    std::cout << "Shutting down socket." << std::endl;
    shutdown(client_socket, SHUT_RDWR);

}