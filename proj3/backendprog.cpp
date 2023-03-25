// memset(), c_str()
#include <cstring>
// perror()
#include <cerrno>
// cerr, cout, endl
#include <iostream>
// IPPROTO_UDP
#include <netinet/in.h>
// istringstream, good()
#include <sstream>
// pid_t
#include <sys/types.h>
// close()
#include <unistd.h>
#include <unordered_set>

#include "backendprog.h"

// the folliwing header files are included in BackendProg.h:
// <fstream>, <netdb.h>, <string>, <sys/socket.h>, <unordered_map>

// typedef std::unordered_map<std::string, DepInfo> servmap;
// private instance variables:
// int sockfd;
// std::string backend_id;
// const char *filename;
// const char *backend_port;
// const char *server_port;
// servmap serverdata;


// ###########  public static constants  ###########
const char *BackendProg::HOST_IP = "127.0.0.1";
const int BackendProg::BUFFER_SIZE = 21;


// ###########  constructor  ###########
BackendProg::BackendProg(
  const std::string backend_id, 
  const char *filename, 
  const char *backend_port, 
  const char *server_port
) {
  this->backend_id = backend_id;
  this->filename = filename;
  this->backend_port = backend_port;
  this->server_port = server_port;
}


// ###########  public methods  ###########
bool BackendProg::setup_server()
{
  struct addrinfo *backend_info;
  struct addrinfo *server_info;
  
  if (
    // fill backend_info with backend values needed to establish a socket
    this->get_addr(backend_info, this->backend_port) 
    // create the socket, modifying file descriptor pointed to by sockfd
    && this->create_socket(backend_info)
    // bind the newly created socket to the specified port
    && this->bind_socket(backend_info)
    // fill server_info with destination values to be used by connect()
    && this->get_addr(server_info, this->server_port)
    // "connect" to destinastion address, so that dest addr will be auto filled
    && this->UDP_connect(server_info)
  ) {
    // done with addrinfo
    freeaddrinfo(backend_info);
    freeaddrinfo(server_info);
    std::cout 
      << "Server " 
      << this->backend_id
      << " is up and running using UDP on port "
      << this->backend_port
      << std::endl;
    // read in data
    std::ifstream infile(this->filename);
    this->read_file(infile);
    return true;
  }
  freeaddrinfo(backend_info);
  freeaddrinfo(server_info);
  return false;
}


void BackendProg::run_server()
{
  // keep getting main server queries
  while (this->respond_to_query()) {}
  // close the socket
  close(this->sockfd);
}


// ###########  private methods  ###########
// https://stackoverflow.com/questions/8960055/getline-to-string-copies-newline-as-well
void BackendProg::getline_no_cr(std::istream& input, std::string& line)
{
  std::getline(input, line);
  if (line.length() > 0 && *(line.rbegin()) == '\r') {
    line.pop_back();
  }
}


std::string BackendProg::extract_unique_ids(const std::string &line)
{
  int oss_len;
  std::string single_id;
  std::string unique_id_num;
  std::unordered_set<std::string> unique_ids;
  std::istringstream iss;
  std::ostringstream oss;

  iss.str(line);
  // extract students IDs and insert them into set
  while (iss.good()) {
    std::getline(iss, single_id, ',');
    unique_ids.insert(single_id);
  }
  // get number of unique IDs as a string
  unique_id_num = std::to_string(unique_ids.size());
  // get all unique IDs as a ", "-separated string
  for (const std::string &stud_id: unique_ids) {
    oss << stud_id << ", ";
  }
  // attach unique_id_num to the end of oss with a period
  // this will overwrite the last ", "
  // https://cplusplus.com/reference/ostream/ostream/tellp/
  oss_len = oss.tellp();
  oss.seekp(oss_len - 2);
  oss << "." << unique_id_num;

  return oss.str();
}


void BackendProg::read_file(std::istream& input)
{
  std::string line;
  std::string dep_list;
  std::string dep_name;
  std::ostringstream oss;

  while (!input.eof()) {
    // get department name
    this->getline_no_cr(input, line);
    dep_name = line;
    // put department name into oss
    oss << dep_name << ",";

    // get student IDs for this department and store in serverdata
    this->getline_no_cr(input, line);
    std::string dep_info = this->extract_unique_ids(line);
    (this->serverdata).emplace(dep_name, dep_info);
  }
  // std::cout << "server reads from " << this->filename << std::endl;
  dep_list = oss.str();
  // remove trailing comma
  dep_list.pop_back();
  // send department list
  this->send_dep_list(dep_list);
}

// https://beej.us/guide/bgnet/html/
bool BackendProg::get_addr(
  struct addrinfo *& addr_info, 
  const char *port_number
) {
  struct addrinfo hints;
  int status;

  // initialize the hints object
  memset(&hints, 0, sizeof hints);
  // use IPv4
  hints.ai_family = AF_INET;
  // choose UDP datagram
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
bool BackendProg::create_socket(const struct addrinfo *backend_info)
{
  this->sockfd = socket(
    backend_info->ai_family, 
    backend_info->ai_socktype, 
    backend_info->ai_protocol
  );
  if (this->sockfd == -1) {
    perror("Socket creation error");
    return false;
  }
  return true;
}

// https://beej.us/guide/bgnet/html/
bool BackendProg::bind_socket(const struct addrinfo *backend_info)
{
  int bind_status;

  bind_status = bind(
    this->sockfd, 
    backend_info->ai_addr, 
    backend_info->ai_addrlen
  );
  if (bind_status == -1) {
    perror("Socket binding error");
    return false;
  }
  return true;
}


bool BackendProg::UDP_connect(const struct addrinfo *server_info)
{
  int connect_status;

  connect_status = connect(
    this->sockfd, 
    server_info->ai_addr, 
    server_info->ai_addrlen
  );
  if (connect_status == -1) {
    perror("Backend UDP connect error");
    return false;
  }
  return true;
}


void BackendProg::send_dep_list(const std::string &dep_list)
{
  char buffer[1];
  ssize_t msg_size;

  msg_size = recv(this->sockfd, buffer, 1, 0);
  if (msg_size < 0) {
    perror("Backend dep list req recv error");
    return;
  }

  if (std::string(buffer, msg_size) != "0") {
    std::cerr << "Backend dep list req recv incorrectly" << std::endl;
    return;
  }

  msg_size = send(this->sockfd, dep_list.c_str(), dep_list.length(), 0);
  if (msg_size < 0) {
    perror("Backend dep list send error");
    return;
  }

  std::cout 
    << "Server "
    << this->backend_id
    << " has sent a department list to Main Server"
    << std::endl;
}


std::string BackendProg::recv_dep_name()
{
  char buffer[BUFFER_SIZE];
  ssize_t msg_size;
  std::string dep_name;

  msg_size = recv(this->sockfd, buffer, BUFFER_SIZE, 0);
  if (msg_size < 0) {
    perror("Backend recv error");
    return "";
  }

  dep_name = std::string(buffer, msg_size);
  std::cout 
    << "Server has received a request for "
    << dep_name
    << std::endl;
  return dep_name;
}


void BackendProg::send_stud_ids(const std::string &stud_id_info)
{
  ssize_t msg_size;

  msg_size = send(this->sockfd, stud_id_info.c_str(), stud_id_info.length(), 0);
  if (msg_size < 0) {
    perror("Server send error");
    return;
  }

  std::cout 
    << "Server "
    << this->backend_id
    << " has sent the results to Main Server"
    << std::endl;
}


bool BackendProg::respond_to_query()
{
  std::string stud_id_info;
  std::string stud_ids;
  std::string unique_id_num;
  std::string dep_name;
  size_t dot_pos;

  dep_name = this->recv_dep_name();

  // if dep_name length is 0, the server has disconnected
  if (dep_name.length() == 0) {
    return false;
  }
  // find dep_name in serverdata
  servmap::const_iterator iter = this->serverdata.find(dep_name);
  if (iter == this->serverdata.cend()) {
    // if not find this department, set stud_id_info to "."
    stud_id_info = ".";
  } else {
    // if found, print and send result
    stud_id_info = iter->second;
    dot_pos = stud_id_info.find(".");
    // print on-screen message
    std::cout 
      << "Server found "
      << stud_id_info.substr(dot_pos+1)
      << " distinct students for "
      << dep_name 
      << ": "
      << stud_id_info.substr(0, dot_pos)
      << std::endl;
  }
  // respond to client with server number of dep_name
  this->send_stud_ids(stud_id_info);
  std::cout << "\n";
  return true;
}
