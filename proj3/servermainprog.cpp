// memset()
#include <cstring>
// perror()
#include <cerrno>
// cout, endl
#include <iostream>
// istringstream
#include <sstream>
// ssize_t
#include <sys/types.h>
// close()
#include <unistd.h>
// IPPROTO_UDP
#include <netinet/in.h>
// AF_INET, SOCK_DGRAM, socklen_t, struct sockaddr
// bind(), connect(), sendto(), recvfrom(), socket()
#include <sys/socket.h>

#include "servermainprog.h"
// the folliwing header files are included in ServerMainProgm.h:
// <netdb.h>, <string>

// private instance variables:
// int sockfd;
// const char *server_port
// const char *backend_A_port
// const char *backend_B_port
// struct addrinfo *backend_A_info
// struct addrinfo *backend_B_info
// servmap serverdata


// ###########  public static constants  ###########
const char *ServerMainProgm::HOST_IP = "127.0.0.1";
const int ServerMainProgm::DEP_LIST_BUFFER_SIZE = 210;
const int ServerMainProgm::BUFFER_SIZE = 2203;


// ###########  constructor  ###########
ServerMainProgm::ServerMainProgm(
  const char *server_port,
  const char *backend_A_port,
  const char *backend_B_port
) {
  this->server_port = server_port;
  this->backend_A_port = backend_A_port;
  this->backend_B_port = backend_B_port;
}


// ###########  public methods  ###########
bool ServerMainProgm::setup_server()
{
  struct addrinfo *server_info;
  strvec dep_list_A;
  strvec dep_list_B;

  if (
    this->get_addr(server_info, this->server_port)
    && this->create_socket(server_info)
    && this->bind_socket(server_info)
    && this->get_addr(this->backend_A_addr, this->backend_A_port)
    && this->get_addr(this->backend_B_addr, this->backend_B_port)
  ) {
    freeaddrinfo(server_info);
    std::cout << "Main server is up and running." << std::endl;

    // request department lists from backend servers
    dep_list_A = this->request_dep_list("A");
    dep_list_B = this->request_dep_list("B");
    std::cout << "Server A\n";
    for (const std::string &dep_name: dep_list_A) {
      std::cout << dep_name << "\n";
    }
    std::cout << "\nServerB\n";
    for (const std::string &dep_name: dep_list_B) {
      std::cout << dep_name << "\n";
    }
    std::cout << std::endl;
    return true;
  }
  freeaddrinfo(server_info);
  return false;
}


void ServerMainProgm::run_server()
{
  std::string dep_name;

  while (true) {
    dep_name = "";
    // if user input nothing, keep asking for input
    while (dep_name.length() == 0) {
      std::cout << "Enter Department Name: ";
      dep_name = this->read_input(std::cin);
    }
    // query for dep_name
    if (!this->query_for_dep(dep_name)) break;
    std::cout << "\n-----Start a new query-----" << std::endl;
  }

  close(this->sockfd);
}


// ###########  private methods  ###########
std::string ServerMainProgm::read_input(std::istream& input)
{
  std::string dep_name;

  std::getline(input, dep_name);
  return dep_name;
}

// https://beej.us/guide/bgnet/html/
bool ServerMainProgm::get_addr(
  struct addrinfo *& addr_info, 
  const char *port_number
) {
  struct addrinfo hints;
  int status;

  // initialize the hints object
  memset(&hints, 0, sizeof hints);
  // use IPv4
  hints.ai_family = AF_INET;
  // choose UDP
  hints.ai_socktype = SOCK_DGRAM;
  // use UDP
  hints.ai_protocol = IPPROTO_UDP;

  status = getaddrinfo(HOST_IP, port_number, &hints, &addr_info);
  if (status != 0) {
    perror(gai_strerror(status));
    return false;
  }
  return true;
}

// https://beej.us/guide/bgnet/html/
bool ServerMainProgm::create_socket(const struct addrinfo *server_info)
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
bool ServerMainProgm::bind_socket(const struct addrinfo *server_info)
{
  int bind_status;

  bind_status = bind(
    this->sockfd, 
    server_info->ai_addr, 
    server_info->ai_addrlen
  );
  if (bind_status == -1) {
    perror("Socket binding error");
    return false;
  }
  return true;
}


strvec ServerMainProgm::request_dep_list(const std::string &backend_id)
{
  char buffer[DEP_LIST_BUFFER_SIZE];
  ssize_t msg_size;
  std::istringstream iss;
  std::string dep_name;
  strvec dep_list;
  const struct addrinfo *backend_info = 
    (backend_id == "A") ? this->backend_A_addr : this->backend_B_addr;

  msg_size = sendto(
    this->sockfd, 
    "0", 
    1, 
    0, 
    backend_info->ai_addr, 
    backend_info->ai_addrlen
  );
  if (msg_size < 0) {
    perror("Server req dep list error");
    return dep_list;
  }

  msg_size = recvfrom(
    this->sockfd, 
    buffer,
    DEP_LIST_BUFFER_SIZE,
    0,
    NULL,
    NULL
  );
  if (msg_size < 0) {
    perror("Server dep list recv error");
    return dep_list;
  }

  iss.str(std::string(buffer, msg_size));
  while (iss.good()) {
    std::getline(iss, dep_name, ',');
    this->serverdata.emplace(dep_name, backend_id);
    dep_list.push_back(dep_name);
  }
  std::cout
    << "Main server has received the department list from server "
    << backend_id
    << " using UDP over port "
    << this->server_port
    << std::endl;
  return dep_list;
}


bool ServerMainProgm::send_dep_name(
  const std::string &dep_name,
  const std::string &backend_id
) {
  ssize_t msg_size;
  const struct addrinfo *backend_info = 
    (backend_id == "A") ? this->backend_A_addr : this->backend_B_addr;

  msg_size = sendto(
    this->sockfd, 
    dep_name.c_str(), 
    dep_name.length(), 
    0,
    backend_info->ai_addr,
    backend_info->ai_addrlen
  );
  if (msg_size < 0) {
    perror("Server send dep name error");
    return false;
  }

  std::cout 
    << "The Main Server has sent request for " 
    << dep_name
    << " to server "
    << backend_id
    << " using UDP over port "
    << this->server_port
    << std::endl;
  return true;
}


void ServerMainProgm::recv_stud_ids(
  const std::string &dep_name,
  const std::string &backend_id
) {
  char buffer[BUFFER_SIZE];
  ssize_t msg_size;
  std::string stud_id_info;
  size_t dot_pos;

  msg_size = recvfrom(this->sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
  if (msg_size < 0) {
    perror("Server recv error");
    return;
  }

  std::cout
    << "The Main server has received searching result(s) of "
    << dep_name
    << " from server "
    << backend_id
    << std::endl;

  stud_id_info = std::string(buffer, msg_size);
  dot_pos = stud_id_info.find(".");

  std::cout
    << "There are "
    << stud_id_info.substr(dot_pos+1)
    << " distinct students in "
    << dep_name
    << ". Their IDs are "
    << stud_id_info.substr(0, dot_pos)
    << std::endl;
}


bool ServerMainProgm::query_for_dep(const std::string &dep_name)
{
  std::string backend_id;

  servmap::const_iterator iter = this->serverdata.find(dep_name);
  // dep_name not in backend servers
  if (iter == this->serverdata.cend()) {
    std::cout << dep_name << " does not show up in server A&B" << std::endl;
    return true;
  }
  backend_id = iter->second;
  std::cout << dep_name << " shows up in server " << backend_id << std::endl;

  if (this->send_dep_name(dep_name, backend_id)) {
    this->recv_stud_ids(dep_name, backend_id);
    return true;
  }
  return false;
}
