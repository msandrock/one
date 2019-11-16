#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace fs = std::filesystem;

fs::path integration_path() {
    return fs::current_path()
                .parent_path()  // src
                .parent_path(); // build
}

fs::path server_path() {
    return integration_path()
            .parent_path()  // src
            .parent_path(); // build
}

fs::path socket_path() {
    return fs::current_path() / std::string("one.sock");
}

fs::path data_path() {
    return integration_path() / "data/";
}

void clear_data() {
    auto dataPath = data_path();

    fs::remove_all(dataPath);
    fs::create_directory(dataPath);
}

void server(const std::string& serverCommand, const std::string& dataPath) {
    std::string command(serverCommand + " " + dataPath);    //  + "& echo $!"
    std::cout << "Running server: " << command << std::endl;
    int status = std::system(command.c_str());

    std::cout << "Server exited with status " << status << std::endl;
}

std::thread run_server() {
    std::thread s(server, server_path() / "one", data_path());
    // Wait a second for the server to come up
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return s;
}

int connect_socket() {
    struct sockaddr_un addr;
    int fd;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path().c_str(), sizeof(addr.sun_path)-1);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect error");
        exit(-1);
    }

    return fd;
}

void send_command(int socket, const std::string& command) {
    char buf[100];
    int bytes;

    sprintf(buf, "%s\n", command.c_str());
    bytes = strlen(buf);

    if (write(socket, buf, bytes) != bytes) {
        if (bytes > 0) fprintf(stderr, "partial write");
        else {
            perror("write error");
            exit(-1);
        }
    }
}

std::string read_response(int socket) {
    char buffer[1024];
    int length = read(socket, buffer, 1024);
    // Terminate the string on \n
    buffer[length - 1] = '\0';

    return std::string(buffer);
}

void close_connection(int socket) {
    close(socket);
}
