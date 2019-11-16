#ifndef INTEGRATIONTEST_HPP
#define INTEGRATIONTEST_HPP
#include <filesystem>
#include <string>
#include <thread>

namespace fs = std::filesystem;

fs::path integration_path();
fs::path server_path();
fs::path socket_path();
fs::path data_path();

void clear_data();
void server(const std::string& serverCommand, const std::string& dataPath);
std::thread run_server();
int connect_socket();
void send_command(int socket, const std::string& command);
void close_connection(int socket);

#endif