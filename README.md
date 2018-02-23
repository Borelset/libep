# libep 
基于epoll实现的reactor模式网络库。

### install
```
source build.sh
make
sudo make install
```

### sample
```
void messageCallback(std::weak_ptr<NetModule::TCPConnection> tcpc, time_t time){
  //process
}

int main() {
    NetModule::TCPServer tcpServer(1234);
    tcpServer.setMessageCallback(messageCallback);
    tcpServer.start();

    return 0;
}
```
