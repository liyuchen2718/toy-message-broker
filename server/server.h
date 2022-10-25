#ifndef SERVER_H
#define SERVER_H

#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "epoller.h" 
#include "../pool/threadpool.h" 
#include "../conn/conn.h"

class Server {
public:
    Server(
        int port, int trigMode, int timeoutMS, bool OptLinger, int threadNum);

    ~Server();
    void Start();

private:
    bool InitSocket_(); 
    void InitEventMode_(int trigMode);
    void AddClient_(int fd, sockaddr_in addr);
  
    void DealListen_();
    void DealWrite_(Conn* client);
    void DealRead_(Conn* client);

    void SendError_(int fd, const char*info);
    void CloseConn_(Conn* client);

    void OnRead_(Conn* client);
    void OnWrite_(Conn* client);
    void OnProcess(Conn* client);

    static const int MAX_FD = 65536;

    static int SetFdNonblock(int fd);

    int port_;
    bool openLinger_;
    int timeoutMS_;  
    bool isClose_;
    int listenFd_;
 
    
    uint32_t listenEvent_;
    uint32_t connEvent_;
    
    std::unique_ptr<ThreadPool> threadpool_;
    std::unique_ptr<Epoller> epoller_;
    std::unordered_map<int, Conn> users_;
};


#endif //SERVER_H