//for STL
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
//for boost
#include <boost/asio.hpp>
#include <boost/array.hpp>
//for db_connection
#include "db_conn.hpp"
//for chat application packet
#include "chat_packet.hpp"
using boost::asio::ip::tcp;
class chat_session:
	public std::enable_shared_from_this<chat_session>{
	public:
		chat_session(tcp::socket sock)
			:socket_(std::move(sock)){
		}
		~chat_session(){
			socket_.close();
		}
		void start(){
			do_read_header();
		}
		/*
		void close(){
			
		}
		*/
	private:
		int do_read_header(){
			auto self(shared_from_this());
			boost::asio::async_read(socket_,
					boost::asio::buffer(packet_buf.data(),chat_packet::header_length),
					[this,self](boost::system::error_code ec, std::size_t size){
				if(!ec){
					std::cout << std::string(packet_buf.data(),chat_packet::header_length) << std::endl;
					switch(packet_buf.decode_header()){
						case Operation::Log_in :
							do_login();
							break;
							/*
						case Operation::Room_list :
							get_room_list();
							break;
						case Operation::In_room :
							set_room(packet_buf.);
							break;
						case Operation::Out_room :
							set_room(0);
							break;
						case Operation::Message :
							do_read_body();
							break;
						case Operation::Log_out :
							close();
							break;
							*/
						default:
							do_read_header();
					}
				}else{
					std::cerr << ec.message() << std::endl;
				}
			});
		}
		void do_login(){
			std::cout << "Do login" << std::endl;
			auto self (shared_from_this());
			boost::asio::async_read(socket_,
					boost::asio::buffer(packet_buf.body(),packet_buf.body_length()),
					[this,self](boost::system::error_code ec, std::size_t size){
				if(!ec){
					send_hash();				
				}
			});
		}
		void send_hash(){
			std::cout << "Do send hash " <<std::endl;
			auto self (shared_from_this());
			std::string id,pwd,hash="";
			db_conn csdb("127.0.0.1","3306","root","jackthe7582@");
			std::cout << std::string(packet_buf.data(),packet_buf.length()) << std::endl;
			std::string packet_body(packet_buf.body(),packet_buf.body_length());
			std::stringstream sstr(packet_body);
			sstr >> id;
			sstr >> pwd;
			//need more improvement in query statement
			std::cout << id <<":" << pwd << std::endl;
			if(!id.empty()&&!pwd.empty()){
				std::string query = "select id,pwd from Users where name='"+id+"';";
				sql::ResultSet *rs = csdb.setQuery("csdb",query);
				if(rs->next() && pwd == rs->getString("pwd")){
					hash = rs->getString("id");
				}
			}
			if(!hash.empty()){
				packet_buf.write("c"+std::to_string(hash.size()),hash);
			}else{
				packet_buf.write("b"+std::to_string(hash.size()),hash);
			}
			//need more improvement in hashing
			boost::asio::async_write(socket_,
				boost::asio::buffer(packet_buf.data(),packet_buf.length()),
				[this,self](boost::system::error_code ec, std::size_t size){
				if(!ec){
					do_read_header();	
				}else{
					//close();
				}
			});
		}
		tcp::socket socket_;
		chat_packet packet_buf;
};
