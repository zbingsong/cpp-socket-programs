// memset(), c_str()
#include <cstring>
// perror()
#include <cerrno>
// cerr, cout, endl
#include <iostream>
// IPPROTO_UDP
#include <netinet/in.h>
// istringstream, ostringstream, good()
#include <sstream>
// pid_t
#include <sys/types.h>
// close()
#include <unistd.h>
// accumulate()
#include <numeric>
// setprecision()
#include <iomanip>
// fixed
#include <ios>
// pair
#include <utility>

#include "backendprog.h"


// ###########  public static constants  ###########
const char *BackendProg::SERVER_PORT = "32378";
const char *BackendProg::HOST_IP = "127.0.0.1";
const int BackendProg::BUFFER_SIZE = 32;


// ###########  constructor  ###########
BackendProg::BackendProg(
  const std::string backend_id, 
  const char *filename, 
  const char *backend_port
) {
  this->backend_id = backend_id;
  this->filename = filename;
  this->backend_port = backend_port;
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
    && this->get_addr(server_info, SERVER_PORT)
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
    // std::cout << "prepare to read file" << std::endl;
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
void BackendProg::process_single_gpa(const std::vector<int>& stud_scores, const int stud_id, scoremap& score_map)
{
  int total;
  float gpa;
  scoremap::iterator iter_score;

  total = std::accumulate(stud_scores.begin(), stud_scores.end(), 0);
  gpa = (float) total / stud_scores.size();
  // put this gpa and student into dep_score_map
  iter_score = score_map.find(gpa);
  if (iter_score == score_map.end()) {
    score_map.emplace(gpa, std::vector<int> (1, stud_id));
  } else {
    (iter_score->second).push_back(stud_id);
  }
}


void BackendProg::process_dep_name_token(const std::string& dep_name, depscoremap& dep_score_map, depsizemap& dep_size)
{
  depscoremap::const_iterator iter_dep_score;
  depsizemap::iterator iter_dep_size;

  iter_dep_score = dep_score_map.find(dep_name);
  // get a scoremap for this department, or a new scoremap if department
  // has not been added into dep_score_map
  if (iter_dep_score == dep_score_map.cend()) {
    dep_score_map.emplace(dep_name, scoremap());
  }
  // also increment department size
  iter_dep_size = dep_size.find(dep_name);
  if (iter_dep_size == dep_size.end()) {
    dep_size.emplace(dep_name, 1);
  } else {
    iter_dep_size->second ++;
  }
}


void BackendProg::process_line_input(std::string& line, depscoremap& dep_score_map, depsizemap& dep_size)
{
  std::string token;
  std::string dep_name;
  int stud_id;
  int score;
  scoremap score_map;
  std::vector<int> stud_scores;

  std::istringstream iss;
  // first token is department name
  iss.str(line);
  std::getline(iss, dep_name, ',');
  // get a scoremap for this department, or a new scoremap if department
  // has not been added into dep_score_map
  this->process_dep_name_token(dep_name, dep_score_map, dep_size);
  // std::cout << token << std::endl;

  // second token is student ID
  std::getline(iss, token, ',');
  stud_id = std::stoi(token);
  // std::cout << stud_id << std::endl;

  // all other tokens are scores if not empty string
  while (iss.good()) {
    std::getline(iss, token, ',');
    if (token.length() == 0) {
      continue;
    }
    score = std::stoi(token);
    stud_scores.push_back(score);
  }
  // compute gpa for student in this department
  this->process_single_gpa(stud_scores, stud_id, dep_score_map.find(dep_name)->second);
  // std::cout << "processed student scores" << std::endl;
}


void BackendProg::generate_dep_stats(const std::string& dep_name, const scoremap& score_map, const float& dep_total, const int& num_stud)
{
  float dep_min_gpa;
  float dep_max_gpa;
  float dep_avg_gpa;
  float dep_variance_gpa;
  float score;
  std::vector<int> stud_list;
  std::ostringstream oss;

  // std::cout << "start getting stats for " << dep_name << std::endl;
  // score_map's keys are ordered, can directly get min and max gpa
  dep_min_gpa = (score_map.cbegin())->first;
  dep_max_gpa = (score_map.crbegin())->first;
  // std::cout << dep_name << " min " << dep_min_gpa << " max " << dep_max_gpa << std::endl;
  // get average
  dep_avg_gpa = dep_total / num_stud;
  // std::cout << dep_name << " avg " << dep_avg_gpa << std::endl;
  // get variance
  dep_variance_gpa = 0;
  for (const auto& pair_score_map: score_map) {
    score = pair_score_map.first;
    stud_list = pair_score_map.second;
    dep_variance_gpa += (score - dep_avg_gpa) * (score - dep_avg_gpa) * stud_list.size();
  }
  dep_variance_gpa /= num_stud;
  // std::cout << dep_name << " variance " << dep_variance_gpa << std::endl;
  // store department gpa statistics in dep_gpa
  oss 
    << std::fixed 
    << std::setprecision(1) 
    << "Department GPA Mean: "
    << dep_avg_gpa 
    << "\nDepartment GPA Variance: " 
    << dep_variance_gpa 
    << "\nDepartment Max GPA: " 
    << dep_max_gpa 
    << "\nDepartment Min GPA: " 
    << dep_min_gpa
    << "\n";
  this->dep_gpa.emplace(dep_name, oss.str());
  // std::cout << dep_name << " store" << std::endl;
}


void BackendProg::generate_recommendations(const studrankmap& stud_ranks)
{
  int stud_id1;
  int stud_id2;
  rankmap rank_map1;
  rankmap rank_map2;
  rankmap::const_iterator iter_rank_map2;
  float min_score;
  int rec_stud_id;
  float cumul_score;
  std::string dep_name;
  float rank1;
  float rank2;

  // get a student
  for (const auto& pair_stud1: stud_ranks) {
    stud_id1 = pair_stud1.first;
    rank_map1 = pair_stud1.second;
    min_score = 200002;
    rec_stud_id = -1;

    // get another student
    for (const auto& pair_stud2: stud_ranks) {
      stud_id2 = pair_stud2.first;
      // if they are the same student, do nothing
      if (stud_id1 == stud_id2) {
        continue;
      }
      rank_map2 = pair_stud2.second;
      cumul_score = 0;
      // for each department attended by student 1, compute square of difference
      // in percentage ranks of two students, and add it to cumul_score
      for (const auto& pair_dep_rank: rank_map1) {
        dep_name = pair_dep_rank.first;
        rank1 = pair_dep_rank.second;
        iter_rank_map2 = rank_map2.find(dep_name);
        if (iter_rank_map2 == rank_map2.cend()) {
          rank2 = 0;
        } else {
          rank2 = iter_rank_map2->second;
        }
        cumul_score += (rank1 - rank2) * (rank1 - rank2);
      }
      // if cumul_score is less than previous, record this new score and id of
      // student 2 (who is potentially the recommended student)
      if (cumul_score < min_score) {
        min_score = cumul_score;
        rec_stud_id = stud_id2;
      } else if (cumul_score == min_score && stud_id2 < rec_stud_id) {
        rec_stud_id = stud_id2;
      }
    }

    // now rec_stud_id should be the id of the most similar student
    // record it
    std::ostringstream oss;
    if (rec_stud_id == -1) {
      oss << "Friend Recommendation: N/A";
    } else {
      oss
        << "Friend Recommendation: "
        << rec_stud_id
        << "\n";
    }
    this->recommendations.emplace(stud_id1, oss.str());
    // std::cout << stud_id1 << ": " << this->recommendations.find(stud_id1)->second;
  }
}


void BackendProg::generate_server_data(const depscoremap& dep_score_map, const depsizemap& dep_size)
{
  int num_stud;
  int num_stud_below;
  float score;
  float dep_total;
  float percentage_rank;
  std::string dep_name;
  std::vector<int> stud_list;
  scoremap score_map;
  studrankmap stud_ranks;
  studrankmap::iterator iter_stud_rank;

  for (const auto& pair_dep_score: dep_score_map) {
    dep_name = pair_dep_score.first;
    score_map = pair_dep_score.second;
    num_stud = (dep_size.find(dep_name))->second;
    num_stud_below = 0;
    dep_total = 0;

    for (const auto& pair_score_map: score_map) {
      score = pair_score_map.first;
      stud_list = pair_score_map.second;
      // add the gpa times the number of students who have this gpa
      dep_total += score * stud_list.size();

      for (const int& stud_id: stud_list) {
        // compute percentage rank and store gpa and rank in dep_stud_stats
        percentage_rank = (float) num_stud_below / num_stud * 100;
        std::ostringstream oss;
        oss 
          << std::fixed 
          << std::setprecision(1) 
          << "Student GPA: "
          << score 
          << "\nPercentage Rank: " 
          << percentage_rank 
          << "%\n";
        this->dep_stud_stats.emplace(dep_name + "," + std::to_string(stud_id), oss.str());

        // store percentage rank in stud_ranks
        iter_stud_rank = stud_ranks.find(stud_id);
        if (iter_stud_rank == stud_ranks.end()) {
          stud_ranks.emplace(stud_id, rankmap ({{dep_name, percentage_rank}}));
        } else {
          (iter_stud_rank->second).emplace(dep_name, percentage_rank);
        }
      }

      num_stud_below += stud_list.size();
    }
    // get stats of this department
    // std::cout << "get stats for department " << dep_name << std::endl;
    this->generate_dep_stats(dep_name, score_map, dep_total, num_stud);
  }
  // std::cout << "get recommendations" << std::endl;
  // get recommendations
  this->generate_recommendations(stud_ranks);
  /*for (const auto& pair_rec: this->recommendations) {
    std::cout << pair_rec.first << ": " << pair_rec.second;
  }*/
}


void BackendProg::read_file(std::istream& input)
{
  std::string line;
  std::string dep_list;
  depscoremap dep_score_map;
  depsizemap dep_size;
  std::ostringstream oss;

  // ignore the first line
  std::getline(input, line);

  while (!input.eof()) {
    // std::cout << "read a line" << std::endl;
    std::getline(input, line);
    if (line.empty()) {
      break;
    }
    line.pop_back();
    this->process_line_input(line, dep_score_map, dep_size);
    // std::cout << "done for a line" << std::endl;
  }
  // std::cout << "finished reading file" << std::endl;
  for (const auto& pair_dep_size: dep_size) {
    oss << pair_dep_size.first << "\n";
  }
  dep_list = oss.str();
  // this->send_dep_list(dep_list);
  // std::cout << "finished sending department list" << std::endl;
  this->generate_server_data(dep_score_map, dep_size);
  // std::cout << "finished generating server data" << std::endl;
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


void BackendProg::send_dep_list(const std::string& dep_list)
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


void BackendProg::recv_query(std::string& dep_name, int& stud_id)
{
  char buffer[BUFFER_SIZE];
  ssize_t msg_size;
  size_t pos;
  std::string message;

  msg_size = recv(this->sockfd, buffer, BUFFER_SIZE, 0);
  if (msg_size < 0) {
    perror("Backend recv error");
    dep_name = "";
    stud_id = -1;
  }

  message = std::string(buffer, msg_size);
  pos = message.find(",");
  dep_name = message.substr(0, pos);
  stud_id = std::stoi(message.substr(pos+1));
  std::cout 
    << "Server "
    << this->backend_id
    << " has received a request for Student "
    << stud_id
    << " in "
    << dep_name
    << std::endl;
}


void BackendProg::send_response(const std::string& response)
{
  ssize_t msg_size;

  msg_size = send(this->sockfd, response.c_str(), response.length(), 0);
  if (msg_size < 0) {
    perror("Server send error");
    return;
  }
  // std::cout << "message size: " << msg_size << std::endl;
}


bool BackendProg::respond_to_query()
{
  std::string dep_name;
  int stud_id;
  depstudmap::const_iterator iter_dep_stud;
  depstatsmap::const_iterator iter_dep_stats;
  recmap::const_iterator iter_rec;
  std::string response;

  this->recv_query(dep_name, stud_id);

  // if dep_name length is 0, the server has disconnected
  if (dep_name.length() == 0) {
    return false;
  }

  // find student gpa and rank in dep_stud_stats
  iter_dep_stud = this->dep_stud_stats.find(dep_name + "," + std::to_string(stud_id));
  if (iter_dep_stud == this->dep_stud_stats.cend()) {
    // if not find this department, set stud_id_info to "."
    // if found, print and send result
    std::cout 
      << "Student "
      << stud_id
      << " does not show up in "
      << dep_name 
      << std::endl;
    response = "Student " + std::to_string(stud_id) + ": Not found";
    this->send_response(response);
    std::cout
      << "Server "
      << this->backend_id
      << " has sent \""
      << response
      << "\" to Main Server"
      << std::endl;
    return true;
  }
  // find department statistics
  iter_dep_stats = this->dep_gpa.find(dep_name);
  // find recommendation
  iter_rec = this->recommendations.find(stud_id);
  std::cout 
    << "Server "
    << this->backend_id
    << " has calculated following academic statistics for Student "
    << stud_id
    << " in "
    << dep_name
    << ": \n"
    << iter_dep_stud->second
    << iter_dep_stats->second
    << iter_rec->second;
  // respond to main server
  response = iter_dep_stud->second + iter_dep_stats->second + iter_rec->second;
  this->send_response(response);
  std::cout
    << "Server "
    << this->backend_id
    << " has sent the result to Main Server"
    << std::endl;
  return true;
}
