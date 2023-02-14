// ntohs()
#include <arpa/inet.h>
// memset()
#include <cstring>
// perror()
#include <cerrno>
// cout, endl
#include <iostream>
// ssize_t
#include <sys/types.h>
// close()
#include <unistd.h>
// IPPROTO_TCP
#include <netinet/in.h>
// AF_INET, SOCK_STREAM, socklen_t, struct sockaddr
// connect(), recv(), socket(), getsockname()
#include <sys/socket.h>

#include "clientprogm.h"
// the folliwing header files are included in clientprogm.h:
// <netdb.h>, <string>

// private instance variables:
// int sockfd;


// ###########  public static constants  ###########
const char *ClientProgm::SERVER_IP = "127.0.0.1";
const char *ClientProgm::SERVER_PORT = "33378";
const int ClientProgm::BUFFER_SIZE = 22;


// ###########  public methods  ###########
bool ClientProgm::setup_client()
{
  struct addrinfo *server_info;
  // create client socket and connect to server
  if (!this->get_addr(server_info)) return false;
  if (!this->create_socket(server_info)) return false;
  if (!this->connect_to_server(server_info)) return false;
  freeaddrinfo(server_info);
  // send client port to server as client id
  if (!this->send_client_id()) return false;

  std::cout << "Client is up and running." << std::endl;
  return true;
}


void ClientProgm::run_client()
{
  std::string dep_name;

  while (true) {
    dep_name = "";
    // if user input nothing, keep asking for input
    while (dep_name.length() == 0) {
      std::cout << "Enter Department Name: ";
      dep_name = this->read_input(std::cin);
    }
    
    if (!this->request_from_server(dep_name)) break;
    std::cout 
      << "\n-----Start a new query-----"
      << std::endl;
  }

  close(this->sockfd);
}


// ###########  private methods  ###########
std::string ClientProgm::read_input(std::istream& input)
{
  std::string dep_name;

  std::getline(input, dep_name);
  // std::cout << "input length: " << dep_name.length() << std::endl;
  return dep_name;
}

// https://beej.us/guide/bgnet/html/
bool ClientProgm::get_addr(struct addrinfo *& server_info)
{
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

// https://beej.us/guide/bgnet/html/
bool ClientProgm::create_socket(struct addrinfo *server_info)
{
  this->sockfd = socket(
    server_info->ai_family, 
    server_info->ai_socktype, 
    server_info->ai_protocol
  );
  if (this->sockfd == -1) {
    perror("Socket creation error");
    freeaddrinfo(server_info);
    return false;
  }
  return true;
}

// https://beej.us/guide/bgnet/html/
bool ClientProgm::connect_to_server(struct addrinfo *server_info)
{
  int connect_status;

  connect_status = connect(
    this->sockfd, 
    server_info->ai_addr, 
    server_info->ai_addrlen
  );
  if (connect_status == -1) {
    perror("Client connection error");
    freeaddrinfo(server_info);
    return false;
  }
  return true;
}


bool ClientProgm::send_client_id()
{
  int getsock_status;
  std::string client_port;
  struct sockaddr_in client_addr;
  socklen_t client_addrlen;
  ssize_t message_size;

  // https://gist.github.com/listnukira/4045436
  client_addrlen = sizeof client_addr;
  getsock_status = getsockname(
    this->sockfd, 
    (struct sockaddr *)&client_addr, 
    &client_addrlen
  );
  if (getsock_status == -1) {
    perror("Client get id error");
    return false;
  }
  client_port = std::to_string(ntohs(client_addr.sin_port));

  // std::cout << "client on port " << client_port << std::endl;
  message_size = send(
    this->sockfd, 
    client_port.c_str(), 
    client_port.length(), 
    0
  );
  if (message_size < 0) {
    perror("Client send id error");
    return false;
  }
  return true;
}


bool ClientProgm::send_dep_name(const std::string& dep_name)
{
  ssize_t message_size;

  message_size = send(this->sockfd, dep_name.c_str(), dep_name.length(), 0);
  if (message_size < 0) {
    perror("Client send error");
    return false;
  }
  std::cout 
    << "Client has sent Department " 
    << dep_name
    << " to Main Server using TCP." 
    << std::endl;
  return true;
}


void ClientProgm::recv_serv_num(const std::string& dep_name)
{
  char buffer[BUFFER_SIZE];
  ssize_t message_size;
  std::string serv_num;

  message_size = recv(sockfd, buffer, BUFFER_SIZE, 0);

  if (message_size < 0) {
    perror("Server recv error");
    return;
  }

  serv_num = std::string(buffer, message_size);
  if (serv_num == "-1") {
    std::cout << dep_name << " not found." << std::endl;
  } else {
    std::cout
      << "Client has received results from Main Server: "
      << dep_name
      << " is associated with backend server "
      << serv_num
      << "."
      << std::endl;
  }
}


bool ClientProgm::request_from_server(const std::string& dep_name)
{
  if (this->send_dep_name(dep_name)) {
    this->recv_serv_num(dep_name);
    return true;
  }
  return false;
}
