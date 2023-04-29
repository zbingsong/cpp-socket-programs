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


// ###########  public static constants  ###########
const char *ClientProgm::SERVER_IP = "127.0.0.1";
const char *ClientProgm::SERVER_PORT = "33378";
const int ClientProgm::BUFFER_SIZE = 190;


// ###########  public methods  ###########
bool ClientProgm::setup_client()
{
  struct addrinfo *server_info;
  // create client socket and connect to server
  if (
    this->get_addr(server_info)
    && this->create_socket(server_info)
    && this->connect_to_server(server_info)
    && this->get_client_id()
  ) {
    freeaddrinfo(server_info);
    std::cout << "Client is up and running." << std::endl;
    return true;
  } else {
    freeaddrinfo(server_info);
    return false;
  }
}


void ClientProgm::run_client()
{
  std::string dep_name;
  // keep student id as string bc will send it as string anyway
  std::string stud_id;

  while (true) {
    dep_name = "";
    stud_id = "";
    // if user input nothing, keep asking for input
    while (dep_name.length() == 0) {
      std::cout << "Enter department name: ";
      dep_name = this->read_input(std::cin);
    }
    while (stud_id.length() == 0) {
      std::cout << "Enter student ID: ";
      stud_id = this->read_input(std::cin);
    }
    // query for dep_name
    if (!this->request_from_server(dep_name, stud_id)) break;
    std::cout << "\n-----Start a new query-----" << std::endl;
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
    return false;
  }
  return true;
}


// https://beej.us/guide/bgnet/html/
bool ClientProgm::create_socket(const struct addrinfo *server_info)
{
  this->sockfd = socket(
    server_info->ai_family, 
    server_info->ai_socktype, 
    server_info->ai_protocol
  );
  if (this->sockfd == -1) {
    perror("Socket creation error");
    return false;
  }
  return true;
}


// https://beej.us/guide/bgnet/html/
bool ClientProgm::connect_to_server(const struct addrinfo *server_info)
{
  int connect_status;

  connect_status = connect(
    this->sockfd, 
    server_info->ai_addr, 
    server_info->ai_addrlen
  );
  if (connect_status == -1) {
    perror("Client connection error");
    return false;
  }
  return true;
}


bool ClientProgm::get_client_id()
{
  int getsock_status;
  std::string client_port;
  struct sockaddr_in client_addr;
  socklen_t client_addrlen;

  // https://gist.github.com/listnukira/4045436
  // get client port
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
  this->client_port = std::to_string(ntohs(client_addr.sin_port));
  return true;
}


bool ClientProgm::send_query(const std::string& dep_name, const std::string& stud_id)
{
  ssize_t message_size;
  std::string message;

  message = dep_name + "," + stud_id;

  message_size = send(this->sockfd, message.c_str(), message.length(), 0);
  if (message_size < 0) {
    perror("Client send error");
    return false;
  }

  std::cout 
    << "Client has sent " 
    << dep_name
    << " and Student "
    << stud_id
    << " to Main Server using TCP over port " 
    << this->client_port
    << std::endl;
  return true;
}


void ClientProgm::recv_response(const std::string& dep_name, const std::string& stud_id)
{
  char buffer[BUFFER_SIZE];
  ssize_t message_size;
  std::string message;

  message_size = recv(this->sockfd, buffer, BUFFER_SIZE, 0);

  if (message_size < 0) {
    perror("Server recv error");
    return;
  }

  // std::cout << "message size: " << message_size << std::endl;
  message = std::string(buffer, message_size);
  if (message_size < 32) {
    // not found
    std::cout << message << std::endl;
  } else {
    // found
    std::cout
      << "The performance statistics for Student "
      << stud_id
      << " in "
      << dep_name
      << " is: \n"
      << message
      << std::endl;
  }
}


bool ClientProgm::request_from_server(const std::string& dep_name, const std::string& stud_id)
{
  // only do recv if send is successful
  if (this->send_query(dep_name, stud_id)) {
    this->recv_response(dep_name, stud_id);
    return true;
  }
  return false;
}
