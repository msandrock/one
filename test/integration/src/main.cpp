#include <iostream>
#include "integrationtest.hpp"

void test_socket_file_is_removed() {
    std::cout << "#########################" << std::endl;
    std::cout << __func__ << std::endl;
    std::cout << "#########################" << std::endl << std::endl;

    clear_data();
    auto s = run_server();
    auto socket = connect_socket();
    send_command(socket, "quit");
    close_connection(socket);
    s.join();
    // Check that the socket file is removed
    assert(!fs::exists(socket_path()));
}

void test_root_file_is_created() {
    std::cout << "#########################" << std::endl;
    std::cout << __func__ << std::endl;
    std::cout << "#########################" << std::endl << std::endl;

    clear_data();
    auto s = run_server();
    auto socket = connect_socket();
    send_command(socket, "quit");
    close_connection(socket);
    s.join();
    // Check that the root data file was created
    assert(fs::exists(data_path() / std::string("00000000-0000-0000-0000-000000000000.json")));
}

void test_root_node_is_returned() {
    std::cout << "#########################" << std::endl;
    std::cout << __func__ << std::endl;
    std::cout << "#########################" << std::endl << std::endl;

    clear_data();
    auto s = run_server();
    auto socket = connect_socket();
    send_command(socket, "get");
    auto response = read_response(socket);
    send_command(socket, "quit");
    close_connection(socket);
    s.join();
    // Check that the root node is returned
    assert(response == "***root***");
}

int main(int argc, const char** argv) {
    test_socket_file_is_removed();
    test_root_file_is_created();
    test_root_node_is_returned();
    
    return EXIT_SUCCESS;
} 
