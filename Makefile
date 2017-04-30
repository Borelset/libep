all:
	g++ -std=c++11 main.cpp methods.cpp route.cpp epoll_process.cpp log.cpp thread_pool.cpp -o simple_httpserver -lpthread
