// ifstream
#include <fstream>
// struct addrinfo
// In cpp file:
// AI_PASSIVE, getaddrinfo(), freeaddrinfo(), gai_strerror()
#include <netdb.h>
// string
// In cpp file:
// getline(), length(), rbegin(), erase()
#include <string>
// ssize_t
// In cpp file:
// AF_INET, SOCK_STREAM, struct sockaddr_storage, socklen_t,
// accept(), bind(), listen(), socket(), recv(), send()
#include <sys/socket.h>
// vector
// In cpp file:
// begin()
#include <vector>
// unordered_map
// In cpp file:
// emplace(), end(), find()
#include <unordered_map>


typedef std::unordered_map<std::string, std::string> servmap;
typedef std::vector<std::string> strvec;
typedef std::vector<int> intvec;

class ServerProgm {
  public:
    static const char *SERVER_PORT;
    static const char *DATAFILE;
    static const int BACKLOG;
    static const int BUFFER_SIZE;
    static const int ID_BUFFER_SIZE;
    bool setup_server();
    void run_server();

  private:
    int sockfd;
    strvec serv_nums;
    servmap serverdata;
    void getline_no_cr(std::istream& input, std::string& line);
    void print_list_file(intvec& distinct_dep_nums);
    void read_file(std::istream& input);
    bool get_addr(struct addrinfo *& server_info);
    bool create_socket(struct addrinfo *server_info);
    bool bind_socket(struct addrinfo *server_info);
    bool start_listen(struct addrinfo *server_info);
    bool accept_client_connection(int *client_sockfdptr);
    std::string recv_client_id(const int client_sockfd);
    std::string recv_dep_name(const int client_sockfd, const std::string &client_id);
    void send_serv_num(const int client_sockfd, const std::string& serv_num, const std::string &client_id);
    bool respond_to_client(const int client_sockfd, const std::string &client_id);
};
