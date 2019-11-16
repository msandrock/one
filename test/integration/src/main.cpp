#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace fs = std::filesystem;

void server() {
    std::cout << "Running server" << std::endl;

    /*std::string command(serverCommand + " " + dataPath);
    command += cwd;
    command += "/data";

    std::cout << command << std::endl;
    int status = std::system(command.c_str());

    std::cout << "Server exited with status " << status << std::endl;*/
}

int main(int argc, const char** argv) {
    std::cout << "One integration test suite" << std::endl;

    //
    // Directories
    //

    auto integrationPath = fs::current_path()
        .parent_path()  // src
        .parent_path(); // build

    auto serverPath =  integrationPath
        .parent_path()  // integration
        .parent_path(); // test

    auto serverCommand = serverPath += "/one";
    auto dataPath = integrationPath += "/data/";
    auto socketPath = serverPath += "/one.sock";

    //
    // Start server
    //

    std::thread s(server);
    // Wait a second for the server to come up
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "Running test suite" << std::endl;

    //
    // Connect to socket
    //

    struct sockaddr_un addr;
    char buf[100];
    int fd, rc;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path)-1);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect error");
        exit(-1);
    }

    /*while((rc=read(STDIN_FILENO, buf, sizeof(buf))) > 0) {

        if (write(fd, buf, rc) != rc) {
            if (rc > 0) fprintf(stderr,"partial write");
            else {
                perror("write error");
                exit(-1);
            }
        }
    }*/

    // Write quit command to server


    s.join();
    return EXIT_SUCCESS;
} 
