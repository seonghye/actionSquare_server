//for STL
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
//for boost
#include <boost/asio.hpp>
#include <boost/array.hpp>
//for chat_session
#include "chat_session.hpp"
using boost::asio::ip::tcp;
class chat_server{
	public:
		chat_server(boost::asio::io_service& ios,
				const tcp::endpoint& endp)
			:	acceptor_(ios,endp),
				socket_(ios){
				do_accept();	
			}
	private:
		void do_accept(){
			acceptor_.async_accept(socket_,
					[this](boost::system::error_code ec){
						std::cout << "Do accept" << std::endl;
						if(!ec){
							std::cout << "Connect" << std::endl;
							std::make_shared<chat_session>(std::move(socket_))->start();
						}else{
							std::cout << ec.message() << std::endl;
						}
						do_accept();
					});
		}
		tcp::acceptor acceptor_;
		tcp::socket socket_;
};
int main(int argc, char* argv[]){
	try{
		if(argc<2){
			std::cerr << "Usage: chat_server <port> [ <port> ... ] " <<std::endl;
			return 1;
		}
		boost::asio::io_service ios;
		std::list<chat_server> servers;
		for(int i=1;i<argc;i++){
			tcp::endpoint endp(tcp::v4(), std::atoi(argv[i]));
			servers.emplace_back(ios,endp);
		}
		ios.run();
	}catch(std::exception& e){
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}
