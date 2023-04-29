// exit()
#include <cstdlib>
// memset(), c_str()
#include <cstring>
// perror()
#include <cerrno>
// cout, endl
#include <iostream>
// prev()
#include <iterator>
// IPPROTO_TCP
#include <netinet/in.h>
// istringstream, good()
#include <sstream>
// pid_t
#include <sys/types.h>
// close(), fork()
#include <unistd.h>
#include <mqueue.h>
// vector, begin()
#include <vector>

#include "serverprogm.h"


// ###########  public static constants  ###########
const char *ServerProgm::SERVER_TCP_PORT = "33378";
const char *ServerProgm::SERVER_UDP_PORT = "32378";
const char *ServerProgm::BACKEND_A_PORT = "30378";
const char *ServerProgm::BACKEND_B_PORT = "31378";
const char *ServerProgm::SERVER_IP = "127.0.0.1";
const int ServerProgm::BACKLOG = 5;
const int ServerProgm::QUERY_BUFFER_SIZE = 32;
const int ServerProgm::RESPONSE_BUFFER_SIZE = 190;
const int ServerProgm::DEP_LIST_BUFFER_SIZE = 210;


// ###########  public methods  ###########
bool ServerProgm::setup_server()
{
  struct addrinfo *server_tcp_info;
  struct addrinfo *server_udp_info;
  std::string dep_list_A;
  std::string dep_list_B;
  
  if (
    this->get_udp_addr(server_udp_info, SERVER_UDP_PORT)
    && this->create_socket(server_udp_info, this->udp_sockfd)
    && this->bind_socket(server_udp_info, this->udp_sockfd)
    && this->get_udp_addr(this->backend_A_addr, BACKEND_A_PORT)
    && this->get_udp_addr(this->backend_B_addr, BACKEND_B_PORT)
  ) {
    freeaddrinfo(server_udp_info);
    if (
      this->get_tcp_addr(server_tcp_info) 
      && this->create_socket(server_tcp_info, this->tcp_sockfd)
      && this->bind_socket(server_tcp_info, this->tcp_sockfd)
      && this->start_listen()
    ) {
      freeaddrinfo(server_tcp_info);
      std::cout << "Main server is up and running" << std::endl;
      this->request_dep_list("A", dep_list_A);
      this->request_dep_list("B", dep_list_B);
      std::cout 
        << "Server A\n" 
        << dep_list_A
        << "\nServer B\n"
        << dep_list_B
        << std::endl;
      
      return true;
    }
    freeaddrinfo(server_tcp_info);
    return false;
  }
  freeaddrinfo(server_udp_info);
  return false;
}


void ServerProgm::run_server()
{
  int client_sockfd;
  pid_t process_id;
  
  while (true) {
    // if failed to establish connection with client, exit
    if (!this->accept_client_connection(&client_sockfd)) {
      close(client_sockfd);
      break;
    }

    // if failed to create a new process, drop this client
    process_id = fork();
    if (process_id == -1) {
      perror("Process forking error");
      close(client_sockfd);
      continue;
    }

    if (process_id == 0) {
      // if on child process
      // close the listening socket in child process
      close(this->tcp_sockfd);

      int client_id;
      // https://beej.us/guide/bgnet/html/#getpeernameman
      struct sockaddr_in addr;
      socklen_t len;
      len = sizeof addr;
      getpeername(client_sockfd, (struct sockaddr *)&addr, &len);
      client_id = ntohs((&addr)->sin_port);
      
      // keep getting client queries until client decides to disconnect
      while (this->respond_to_client(client_sockfd, client_id)) {}
      // std::cout << "client disconnected" << std::endl;
      // close the connection to client and exit child process
      close(client_sockfd);
      exit(0);
    } else {
      // if on parent process
      // close the socket connected to client in parent (not used)
      // and continue listening (continue looping)
      close(client_sockfd);
    }
  }
}


// ###########  private methods  ###########
// https://beej.us/guide/bgnet/html/
bool ServerProgm::get_tcp_addr(struct addrinfo *& server_tcp_info)
{
  struct addrinfo hints;
  int status;

  // initialize the hints object
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  status = getaddrinfo(SERVER_IP, SERVER_TCP_PORT, &hints, &server_tcp_info);
  if (status != 0) {
    perror(gai_strerror(status));
    return false;
  }
  return true;
}


bool ServerProgm::get_udp_addr(struct addrinfo *& udp_info, const char *port_number)
{
  struct addrinfo hints;
  int status;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;

  status = getaddrinfo(SERVER_IP, port_number, &hints, &udp_info);
  if (status != 0) {
    perror(gai_strerror(status));
    return false;
  }
  return true;
}


// https://beej.us/guide/bgnet/html/
bool ServerProgm::create_socket(const struct addrinfo *server_info, int& socket_fd)
{
  socket_fd = socket(
    server_info->ai_family, 
    server_info->ai_socktype, 
    server_info->ai_protocol
  );
  if (socket_fd == -1) {
    perror("Socket creation error");
    return false;
  }
  return true;
}


// https://beej.us/guide/bgnet/html/
bool ServerProgm::bind_socket(const struct addrinfo *server_info, int& socket_fd)
{
  int bind_status;

  bind_status = bind(
    socket_fd, 
    server_info->ai_addr, 
    server_info->ai_addrlen
  );
  if (bind_status == -1) {
    perror("Socket binding error");
    return false;
  }
  return true;
}

// https://beej.us/guide/bgnet/html/
bool ServerProgm::start_listen()
{
  int listen_status;

  listen_status = listen(this->tcp_sockfd, BACKLOG);
  if (listen_status == -1) {
    perror("Socket listening error");
    return false;
  }
  return true;
}


void ServerProgm::generate_serverdata(const std::string& backend_id, std::string& dep_list)
{
  std::istringstream iss;
  std::string dep_name;

  iss.str(dep_list);
  while (iss.good()) {
    std::getline(iss, dep_name, '\n');
    this->serverdata.emplace(dep_name, backend_id);
  }
}


void ServerProgm::request_dep_list(const std::string& backend_id, std::string& dep_list)
{
  char buffer[DEP_LIST_BUFFER_SIZE];
  ssize_t msg_size;
  std::vector<std::string> dep_list_vec;
  const struct addrinfo *backend_info = 
    (backend_id == "A") ? this->backend_A_addr : this->backend_B_addr;

  msg_size = sendto(
    this->udp_sockfd, 
    "0", 
    1, 
    0, 
    backend_info->ai_addr, 
    backend_info->ai_addrlen
  );
  if (msg_size < 0) {
    perror("Server req dep list error");
    dep_list = "";
    return;
  }

  msg_size = recvfrom(
    this->udp_sockfd, 
    buffer,
    DEP_LIST_BUFFER_SIZE,
    0,
    NULL,
    NULL
  );
  if (msg_size < 0) {
    perror("Server dep list recv error");
    dep_list = "";
    return;
  }

  dep_list = std::string(buffer, msg_size);
  this->generate_serverdata(backend_id, dep_list);
  std::cout
    << "Main server has received the department list from server "
    << backend_id
    << " using UDP over port "
    << SERVER_UDP_PORT
    << std::endl;
}


// https://beej.us/guide/bgnet/html/
bool ServerProgm::accept_client_connection(int *client_sockfdptr)
{
  struct sockaddr_storage client_addr;
  socklen_t client_addr_size;

  client_addr_size = sizeof client_addr;
  // try to accept client connection
  *client_sockfdptr = accept(
    this->tcp_sockfd, 
    (struct sockaddr *)&client_addr, 
    &client_addr_size
  );
  if (*client_sockfdptr == -1) {
    perror("Client accept error");
    return false;
  }
  // std::cout << "client connected" << std::endl;
  return true;
}


bool ServerProgm::recv_query(const int client_sockfd, const int client_id, std::string& dep_name, std::string& stud_id)
{
  char buffer[QUERY_BUFFER_SIZE];
  ssize_t message_size;
  std::string message;
  size_t pos;

  message_size = recv(client_sockfd, buffer, QUERY_BUFFER_SIZE, 0);
  if (message_size < 0) {
    perror("Server recv error");
    return false;
  }

  // client disconnected
  if (message_size == 0) {
    return false;
  }

  message = std::string(buffer, message_size);
  pos = message.find(",");
  dep_name = message.substr(0, pos);
  stud_id = message.substr(pos+1);
  std::cout 
    << "Main server has received the request on Student " 
    << stud_id
    << " in "
    << dep_name
    << " from client "
    << client_id
    << " using TCP over port "
    << SERVER_TCP_PORT
    << std::endl;
  return true;
}


bool ServerProgm::send_query(const std::string& backend_id, std::string& dep_name, std::string& stud_id)
{
  ssize_t message_size;
  std::string message;
  const struct addrinfo *backend_info = 
    (backend_id == "A") ? this->backend_A_addr : this->backend_B_addr;
  
  message = dep_name + "," + stud_id;
  message_size = sendto(
    this->udp_sockfd, 
    message.c_str(), 
    message.length(), 
    0, 
    backend_info->ai_addr, 
    backend_info->ai_addrlen
  );
  if (message_size < 0) {
    perror("Server send dep name error");
    return false;
  }

  std::cout 
    << "Main Server has sent request of Student "
    << stud_id
    << " to server "
    << backend_id
    << " using UDP over port "
    << SERVER_UDP_PORT
    << std::endl;
  return true;
}


void ServerProgm::recv_response(const std::string& backend_id, const std::string& stud_id, std::string& response, int& flag)
{
  char buffer[RESPONSE_BUFFER_SIZE];
  ssize_t message_size;

  message_size = recvfrom(this->udp_sockfd, buffer, RESPONSE_BUFFER_SIZE, 0, NULL, NULL);
  if (message_size < 0) {
    perror("Server recv error");
    return;
  }

  // std::cout << "message size " << message_size << std::endl;
  response = std::string(buffer, message_size);

  if (message_size < 32) {
    // student id not found
    flag = 2;
    std::cout
      << "Main server has recevied \""
      << response
      <<"\" from server "
      << backend_id
      << std::endl;
  } else {
    // student id found
    flag = 0;
    std::cout
      << "Main server has recevied searching result of Student "
      << stud_id
      <<" from server "
      << backend_id
      << std::endl;
  }
}


void ServerProgm::send_response(const int client_sockfd, const int client_id, const std::string& message, const int flag)
{
  ssize_t message_size;

  message_size = send(client_sockfd, message.c_str(), message.length(), 0);
  if (message_size < 0) {
    perror("Server send error");
    return;
  }

  switch (flag)
  {
    // normal
    case 0:
      std::cout
        << "Main Server has sent searching result(s) to client "
        << client_id
        << " using TCP over port "
        << SERVER_TCP_PORT
        << std::endl;
      break;
    // did not find department name
    case 1:
      std::cout
        << "Main Server has sent \""
        << message
        << " to client "
        << client_id
        << " using TCP over port "
        << SERVER_TCP_PORT
        << std::endl;
    // did not find student id
    case 2:
      std::cout
        << "Main Server has sent message to client "
        << client_id
        << " using TCP over port "
        << SERVER_TCP_PORT
        << std::endl;
    default:
      break;
  }
}


bool ServerProgm::respond_to_client(const int client_sockfd, const int client_id)
{
  std::string dep_name;
  std::string stud_id;
  servmap::const_iterator iter;
  std::string message;
  std::string backend_id;
  int flag;

  // std::cout << "respond to client" << std::endl;
  if (this->recv_query(client_sockfd, client_id, dep_name, stud_id)) {
    // look up department name in serverdata
    iter = this->serverdata.find(dep_name);

    if (iter == serverdata.cend()) {
      // if not find this department, set flag to 1
      flag = 1;
      // print on-screen message
      std::cout 
        << dep_name 
        << " does not show up in server A&B";
      message = dep_name + ": Not found";
    } else {
      // if found, set serv_num to result
      backend_id = iter->second;
      // print on-screen message
      std::cout 
        << dep_name 
        << " shows up in backend server "
        << backend_id 
        << std::endl;
      // forward the query to backend server and receive response
      if (this->send_query(backend_id, dep_name, stud_id)) {
        this->recv_response(backend_id, stud_id, message, flag);
      } else {
        // if did not recv response correctly, stop and return false
        return false;
      }
    }
    // respond to client
    this->send_response(client_sockfd, client_id, message, flag);
    return true;
  }

  // if did not recv the query correctly, return false
  return false;
}
