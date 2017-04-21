all:
	g++ -std=c++11 main.cpp methods.cpp route.cpp epoll_process.cpp -o simple_httpserver
