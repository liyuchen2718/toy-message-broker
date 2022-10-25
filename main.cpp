#include <unistd.h>
#include "server/server.h"

int main() {
 
    std::cout << "start";
    Server server(
        1316, 3, 60000, false, 1024);             
    std::cout << "start" << "listen on 1316";
    server.Start();
} 
