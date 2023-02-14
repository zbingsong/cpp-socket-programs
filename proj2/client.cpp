/*
// memset()
#include <cstring>
// perror()
#include <cerrno>
// cout, endl
#include <iostream>
// struct addrinfo, getaddrinfo(), freeaddrinfo(), gai_strerror()
#include <netdb.h>
// ssize_t
#include <sys/types.h>
// close()
#include <unistd.h>
// IPPROTO_TCP
#include <netinet/in.h>
// AF_INET, SOCK_STREAM, connect(), recv(), socket()
#include <sys/socket.h>


const char *SERVER_IP = "127.0.0.1";
const char *SERVER_PORT = "33378";
const int MAX_BUFFER_SIZE = 24;


void read_input(std::string *dep_nameptr) {
  std::cin >> *dep_nameptr;
}


bool get_addr(struct addrinfo *& server_info) {
  struct addrinfo hints;
  int status;

  // initialize the hints object
  memset(&hints, 0, sizeof hints);
  // use IPv4
  hints.ai_family = AF_INET;
  // choose persistent connection
  hints.ai_socktype = SOCK_STREAM;
  // use TCP
  hints.ai_protocol = IPPROTO_TCP;

  status = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &server_info);
  if (status != 0) {
    perror(gai_strerror(status));
    freeaddrinfo(server_info);
    return false;
  }
  return true;
}


bool create_socket(int *sockfdptr, struct addrinfo *server_info) {
  *sockfdptr = socket(
    server_info->ai_family, 
    server_info->ai_socktype, 
    server_info->ai_protocol
  );
  if (*sockfdptr == -1) {
    perror("Socket creation error");
    freeaddrinfo(server_info);
    return false;
  }
  return true;
}


bool connect_to_server(const int sockfd, struct addrinfo *server_info) {
  int connect_status;

  connect_status = connect(sockfd, server_info->ai_addr, server_info->ai_addrlen);
  if (connect_status == -1) {
    perror("Client connection error");
    freeaddrinfo(server_info);
    return false;
  }
  return true;
}


bool setup_client_socket(int *sockfdptr) {
  struct addrinfo *server_info;

  if (!get_addr(server_info)) return false;
  if (!create_socket(sockfdptr, server_info)) return false;
  if (!connect_to_server(*sockfdptr, server_info)) return false;
  freeaddrinfo(server_info);
  return true;
}


void send_dep_name(const int sockfd, const std::string& dep_name) {
  ssize_t message_size;

  message_size = send(sockfd, dep_name.c_str(), dep_name.length(), 0);
  if (message_size < 0) {
    perror("Client send error");
  }
  std::cout << "size of sent message: " << message_size << std::endl;
}


ssize_t recv_serv_num(const int sockfd, char (&buffer)[MAX_BUFFER_SIZE]) {
  ssize_t message_size;

  message_size = recv(sockfd, buffer, MAX_BUFFER_SIZE, 0);
  if (message_size < 0) {
    perror("Server recv error");
  }
  std::cout << "size of received message: " << message_size << std::endl;
  return message_size;
}


bool request_from_server(
  const int sockfd, 
  const std::string& dep_name, 
  std::string *serv_num
) {
  char buffer[MAX_BUFFER_SIZE];
  ssize_t message_size;

  send_dep_name(sockfd, dep_name);
  message_size = recv_serv_num(sockfd, buffer);

  // server ended connection (should not happen)
  if (message_size == 0) {
    return false;
  }

  if (message_size < 0) {
    perror("Client recv error");
    return true;
  }
  
  *serv_num = std::string(buffer, message_size);
  return true;
}


void run_client(const int sockfd) {
  std::string dep_name;
  std::string serv_num;

  while (true) {
    read_input(&dep_name);


    if (!request_from_server(sockfd, dep_name, &serv_num)) break;

    std::cout << "server number: " << serv_num << std::endl;
  }
}*/

#include "clientprogm.h"


int main() {
  ClientProgm client;

  if (client.setup_client()) {
    client.run_client();
  }

  return 0;
}
