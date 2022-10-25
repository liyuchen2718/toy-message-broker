#include "conn.h"
#include "../queue.h"
using namespace std;

const char* Conn::srcDir;
std::atomic<int> Conn::userCount;
bool Conn::isET;

Conn::Conn() { 
    fd_ = -1;
    addr_ = { 0 };
    isClose_ = true;
};

Conn::~Conn() { 
    Close(); 
};

void Conn::init(int fd, const sockaddr_in& addr) {
    assert(fd > 0);
    userCount++;
    addr_ = addr;
    fd_ = fd;
    writeBuff_.RetrieveAll();
    readBuff_.RetrieveAll();
    isClose_ = false;
    
}

void Conn::Close() {
    //response_.UnmapFile();
    if(isClose_ == false){
        isClose_ = true; 
        userCount--;
        close(fd_);
    }
}

int Conn::GetFd() const {
    return fd_;
};

struct sockaddr_in Conn::GetAddr() const {
    return addr_;
}

const char* Conn::GetIP() const {
    return inet_ntoa(addr_.sin_addr);
}

int Conn::GetPort() const {
    return addr_.sin_port;
}

ssize_t Conn::read(int* saveErrno) {
    ssize_t len = -1;
    do {
        len = readBuff_.ReadFd(fd_, saveErrno);
        if (len <= 0) {
            break;
        }
    } while (isET);
    return len;
}

ssize_t Conn::write(int* saveErrno) {
    ssize_t len = -1;
    do {
        len = writev(fd_, iov_, iovCnt_);
        if(len <= 0) {
            *saveErrno = errno;
            break;
        }
        if(iov_[0].iov_len == 0) { break; }  
        else 
        {
            iov_[0].iov_base = (uint8_t*)iov_[0].iov_base + len; 
            iov_[0].iov_len -= len; 
            writeBuff_.Retrieve(len);
        }
    } while(isET || ToWriteBytes() > 10240);
    return len;
}

bool Conn::process() {
    if(readBuff_.ReadableBytes() <= 0)
    {
        return false;
    } 
    std::string message =readBuff_.RetrieveAllToStr();
    std:size_t Method_GET = message.find("GET");
    std:size_t Method_POST = message.find("POST");
    std::string reply = "Method Not Allowed";
    if (Method_POST !=std::string::npos) {
        threadsafe_queue::getInstance().push(message);
        reply =  "MESSAGE RECEIVED" ;
    }
    else 
    {   
        if (Method_GET != std::string::npos)
        {
            if (!threadsafe_queue::getInstance().try_pop(reply)) {
                reply = "NO MESSAGE";
            }
        }
    }
    writeBuff_.Append(reply);
 
     
    iov_[0].iov_base = const_cast<char*>(writeBuff_.Peek());
    iov_[0].iov_len = writeBuff_.ReadableBytes();
    iovCnt_ = 1;

     
    return true;
}
