#include "Server.h"

int main(int argc, char* argv[])
{   
    char host[32] = "127.0.0.1";
    int port=10086;
    bool timer=false, log=false;
    int ch;
    while((ch = getopt(argc, argv,"h:p:tl")) != -1)
    {
        switch(ch)
        {
            case 'h':
                strcpy(host,optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 't':
                timer = true;
                break;
            case 'l':
                log = true;
                break;
            default:
                printf("Unknown opt\n");
                return 0;
        }
    }
    Server server(host,port, log, timer);
    server.run(); 
    return 0;
}