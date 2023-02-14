// struct addrinfo
// In cpp file:
// getaddrinfo(), freeaddrinfo(), gai_strerror()
#include <netdb.h>
// string, to_string()
#include <string>

class ClientProgm {
  public:
    static const char *SERVER_IP;
    static const char *SERVER_PORT;
    static const int BUFFER_SIZE;
    bool setup_client();
    void run_client();
  private:
    int sockfd;
    std::string read_input(std::istream& input);
    bool get_addr(struct addrinfo *& server_info);
    bool create_socket(struct addrinfo *server_info);
    bool connect_to_server(struct addrinfo *server_info);
    bool send_client_id();
    bool send_dep_name(const std::string& dep_name);
    void recv_serv_num(const std::string& dep_name);
    bool request_from_server(const std::string& dep_name);
};
