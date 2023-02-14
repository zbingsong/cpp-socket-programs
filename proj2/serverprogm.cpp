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

#include "serverprogm.h"

// the folliwing header files are included in serverprogm.h:
// <fstream>, <netdb.h>, <string>, <sys/socket.h>, 
// <unordered_map>, <vector>

// typedef std::unordered_map<std::string, std::string> servmap;
// typedef std::vector<std::string> strvec;
// typedef std::vector<int> intvec;
// private instance variables:
// int sockfd;
// strvec serv_nums;
// servmap serverdata;


// ###########  public static constants  ###########
const char *ServerProgm::SERVER_PORT = "33378";
const char *ServerProgm::DATAFILE = "list.txt";
const int ServerProgm::BACKLOG = 5;
const int ServerProgm::BUFFER_SIZE = 22;
const int ServerProgm::ID_BUFFER_SIZE = 6;


// ###########  public methods  ###########
bool ServerProgm::setup_server()
{
  struct addrinfo *server_info;

  // fill server_info with values needed to establish a server-side socket
  if (!this->get_addr(server_info)) return false;
  // create the socket, modifying file descriptor pointed to by sockfd
  if (!this->create_socket(server_info)) return false;
  // bind the newly created socket to the specified port
  if (!this->bind_socket(server_info)) return false;
  // make socket listen to incoming messages on the port
  if (!this->start_listen(server_info)) return false;
  // done with server_info
  freeaddrinfo(server_info);
  std::cout << "Main server is up and running" << std::endl;
  // read in data
  std::ifstream infile(DATAFILE);
  this->read_file(infile);
  return true;
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

    process_id = fork();
    if (process_id == -1) {
      perror("Process forking error");
      continue;
    }

    if (process_id == 0) {
      // if on child process
      std::string client_id;

      // close the listening socket in child process
      close(sockfd);
      // get client's id
      client_id = recv_client_id(client_sockfd);
      // std::cout << "client id is " << client_id << std::endl;
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
// https://stackoverflow.com/questions/8960055/getline-to-string-copies-newline-as-well
void ServerProgm::getline_no_cr(std::istream& input, std::string& line)
{
  std::getline(input, line);
  if (line.length() > 0 && *(line.rbegin()) == '\r') {
    line.erase(line.length() - 1);
  }
}


void ServerProgm::print_list_file(intvec& distinct_dep_nums)
{
  strvec::const_iterator serv_iter;
  intvec::const_iterator dep_iter;

  std::cout 
    << "Total number of Backend Servers: " 
    << this->serv_nums.size() 
    << std::endl;

  for (
    serv_iter = this->serv_nums.cbegin(), 
      dep_iter = distinct_dep_nums.cbegin(); 
    serv_iter != this->serv_nums.cend(), 
      dep_iter != distinct_dep_nums.cend();
    serv_iter ++, 
      dep_iter ++
  ) {
    std::cout 
      << "Backend Servers " 
      << *serv_iter 
      << " contains " 
      << *dep_iter 
      << " distinct departments" 
      << std::endl;
  }
}


void ServerProgm::read_file(std::istream& input)
{
  std::string line;
  std::string servid;
  std::string dep_name;
  int distinct_dep_counter;
  intvec distinct_dep_nums;

  while (!input.eof()) {
    distinct_dep_counter = 0;
    // get server id
    this->getline_no_cr(input, line);
    servid = line;
    this->serv_nums.push_back(servid);
    // get department as comma-separated string
    this->getline_no_cr(input, line);
    std::istringstream iss(line);
    while (iss.good()) {
      // extract a single department name
      std::getline(iss, dep_name, ',');
      // if this department is not in serverdata, put it in and increment counter
      if ((this->serverdata).find(dep_name) == (this->serverdata).cend()) {
        (this->serverdata).emplace(dep_name, servid);
        distinct_dep_counter ++;
      }
    }
    distinct_dep_nums.push_back(distinct_dep_counter);
  }
  std::cout 
    << "Main server has read the department list from list.txt." 
    << std::endl;
  this->print_list_file(distinct_dep_nums);
}

// https://beej.us/guide/bgnet/html/
bool ServerProgm::get_addr(struct addrinfo *& server_info)
{
  struct addrinfo hints;
  int status;

  // initialize the hints object
  memset(&hints, 0, sizeof hints);
  // use IPv4
  hints.ai_family = AF_INET;
  // choose persistent connection
  hints.ai_socktype = SOCK_STREAM;
  // bind to IP of localhost (server setting)
  hints.ai_flags = AI_PASSIVE;
  // use TCP
  hints.ai_protocol = IPPROTO_TCP;

  status = getaddrinfo(NULL, SERVER_PORT, &hints, &server_info);
  if (status != 0) {
    perror(gai_strerror(status));
    freeaddrinfo(server_info);
    return false;
  }
  return true;
}

// https://beej.us/guide/bgnet/html/
bool ServerProgm::create_socket(struct addrinfo *server_info)
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
bool ServerProgm::bind_socket(struct addrinfo *server_info)
{
  int bind_status;

  bind_status = bind(
    this->sockfd, 
    server_info->ai_addr, 
    server_info->ai_addrlen
  );
  if (bind_status == -1) {
    perror("Socket binding error");
    freeaddrinfo(server_info);
    return false;
  }
  return true;
}

// https://beej.us/guide/bgnet/html/
bool ServerProgm::start_listen(struct addrinfo *server_info)
{
  int listen_status;

  listen_status = listen(this->sockfd, BACKLOG);
  if (listen_status == -1) {
    perror("Socket listening error");
    freeaddrinfo(server_info);
    return false;
  }
  return true;
}

// https://beej.us/guide/bgnet/html/
bool ServerProgm::accept_client_connection(int *client_sockfdptr)
{
  struct sockaddr_storage client_addr;
  socklen_t client_addr_size;

  client_addr_size = sizeof client_addr;
  *client_sockfdptr = accept(
    this->sockfd, 
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


std::string ServerProgm::recv_client_id(const int client_sockfd)
{
  char buffer[ID_BUFFER_SIZE];
  ssize_t message_size;
  std::string client_id;

  message_size = recv(client_sockfd, buffer, ID_BUFFER_SIZE, 0);
  if (message_size < 0) {
    perror("Server recv client id error");
    client_id = "";
  }
  client_id = std::string(buffer, message_size);
  return client_id;
}


std::string ServerProgm::recv_dep_name(
  const int client_sockfd, 
  const std::string &client_id
) {
  char buffer[BUFFER_SIZE];
  ssize_t message_size;
  std::string dep_name;

  message_size = recv(client_sockfd, buffer, BUFFER_SIZE, 0);
  if (message_size < 0) {
    perror("Server recv error");
    return "";
  }
  if (message_size == 0) {
    return "";
  }

  dep_name = std::string(buffer, message_size);
  std::cout 
    << "Main server has received the request on Department " 
    << dep_name
    << " from client "
    << client_id
    << " using TCP over port "
    << SERVER_PORT
    << std::endl;
  return dep_name;
}


void ServerProgm::send_serv_num(
  const int client_sockfd, 
  const std::string &serv_num,
  const std::string &client_id
) {
  ssize_t message_size;

  message_size = send(client_sockfd, serv_num.c_str(), serv_num.length(), 0);
  if (message_size < 0) {
    perror("Server send error");
    return;
  }
  if (serv_num.length() == 0) {
    std::cout 
      << "The Main Server has send \"Department Name: Not found\" to client "
      << client_id
      << " using TCP over port "
      << SERVER_PORT
      << std::endl;
  } else {
    std::cout 
      << "Main Server has send searching result to client "
      << client_id
      << " using TCP over port "
      << SERVER_PORT
      << std::endl;
  }
}


bool ServerProgm::respond_to_client(
  const int client_sockfd,
  const std::string &client_id
) {
  std::string serv_num;
  std::string dep_name;

  dep_name = this->recv_dep_name(client_sockfd, client_id);

  // if dep_name length is 0, the client has ended connection
  if (dep_name.length() == 0) {
    return false;
  }
  // find dep_name in serverdata
  servmap::const_iterator iter = this->serverdata.find(dep_name);
  if (iter == serverdata.cend()) {
    // if not find this department, set serv_num to -1
    serv_num = "-1";
    // print on-screen message
    std::cout 
      << dep_name 
      << " does not show up in backend server ";
    for (
      strvec::const_iterator iter = this->serv_nums.cbegin(); 
      iter != std::prev(this->serv_nums.cend()); 
      iter ++
    ) {
      std::cout << *iter << ", ";
    }
    std::cout << this->serv_nums.back() << std::endl;
  } else{
    // if found, set serv_num to result
    serv_num = iter->second;
    // print on-screen message
    std::cout 
      << dep_name 
      << " shows up in backend server "
      << serv_num 
      << std::endl;
  }
  // respond to client with server number of dep_name
  this->send_serv_num(client_sockfd, serv_num, client_id);
  std::cout << "\n";
  return true;
}
