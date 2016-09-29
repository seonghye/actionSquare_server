all:
	g++ -std=c++11 chat_server.cpp -o chat_server -lboost_system -lmysqlcppconn 2> server_err.txt
	g++ -std=c++11 chat_client.cpp -o chat_client -lboost_system -pthread 2> client_err.txt
server:
	g++ -std=c++11 chat_server.cpp -o chat_server -lboost_system -lmysqlcppconn
client:
	g++ -std=c++11 chat_client.cpp -o chat_client -lboost_system -pthread
run_server:
	sudo ./chat_server 10223
run_client:
	./chat_client ctutkernel.xyz 10223
