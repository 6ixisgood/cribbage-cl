#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>


#define DEFAULT_PORT 8081;

using namespace std; 

void handleNewConnection(int chatSocket);

int main(int argc, char const **argv) {
    short int port = DEFAULT_PORT;
    
    // create a socket
    int sid = socket(PF_INET, SOCK_STREAM, 0);
    checkError(sid);

    // set up address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    // bind the socket to address
    int status = bind(sid, (struct sockaddr*) &addr, sizeof(addr));
    checkError(status);
    // start listening on that socket
    status = listen(sid, 10);
    checkError(status);

    // connection loop
    while(1) {
        struct sockaddr_in client;
        socklen_t clientSize = sizeof(client);
        int chatSocket = accept(sid, (struct sockaddr*) &client, &clientSize);
        cout << "Accepted a socket\n";
        
        // fork to handle new conn
        pid_t child = fork();
        if (child == 0) {
            close(sid);
            handleNewConnection(chatSocket);
            close(chatSocket);
            cout << "Closing\n";
            return -1;
        } else {
            // return to handle more connections
            if (child < 0) {
                cout << "Fork failed\n";
            }
            close(chatSocket);
            int status;
            pid_t deadChild;
            do {
                deadChild = waitpid(0, &status, WNOHANG); checkError(deadChild);
            } while (deadChild > 0);
        }
    }
    return 0;
}

void handleNewConnection(int chatSocket) {
    
}

void checkError(int status) {
    if (status < 0) {
        cout << "ERROR\n";
        exit(-1);
    }
}