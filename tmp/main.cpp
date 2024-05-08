#include "Server.hpp"
#include "EventHandler.hpp"

void	init_kqueue()
{

}

int main(int argc, char **argv) {
	try
	{
		Server IRCServ(argc, argv); //파싱 + 서버객체생성

		EventHandler Handler(IRCServ); //서버를 가진 핸들러객체 생성 -> kqueue를 가진 핸들러가 만들어짐
		Handler.addNewEvent(IRCServ.getSocket(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		//인자는 kevent()의 인자로 쓰이며 socketfd, filter, flags, fflags, data, udata 순서이다.
		//결과로 handler의 kqueue에 서버의 소켓에 대한 listen이벤트가 등록된다.
		//실제 tcp통신은 서버의 listening소켓과 연결되면 새로운 socket을 생성해 클라이언트와 연결하지만 내부적으로 알아서 잘 연결해주는듯
		std::cout << "echo server started" << std::endl;

		while(1) {
			Handler.handleEvents(IRCServ.getSocket());
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
}
