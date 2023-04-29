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
// accept(), bind(), listen(), socket(), recv(), send(), getpeername()
#include <sys/socket.h>
// unordered_map
// In cpp file:
// emplace(), end(), find()
#include <unordered_map>


typedef std::unordered_map<std::string, std::string> servmap;


class ServerProgm {
  public:
    /**
     * port number of main server and backend servers
    */
    static const char *SERVER_TCP_PORT;
    static const char *SERVER_UDP_PORT;
    static const char *BACKEND_A_PORT;
    static const char *BACKEND_B_PORT;

    /**
     * ip address of main server and backend servers
    */
    static const char *SERVER_IP;

    /**
     * number of simultaneous connections to the listening socket
    */
    static const int BACKLOG;

    /**
     * size of buffer to receive query sent by client
    */
    static const int QUERY_BUFFER_SIZE;

    /**
     * size of buffer to receive response from backend server
    */
    static const int RESPONSE_BUFFER_SIZE;

    /**
     * size of buffer to receive department list from backend server
    */
    static const int DEP_LIST_BUFFER_SIZE;

    /**
     * set up server sockets, get department list from backend servers,
     * and listen for incoming connections
     * @return  a boolean indicating if the server is set up correctly
    */
    bool setup_server();

    /**
     * accept client connection, forward client query to backend servers,
     * get response and forward it to client
    */
    void run_server();

  private:
    /**
     * file descriptor for main server sockets
    */
    int udp_sockfd;
    int tcp_sockfd;

    /**
     * addrinfo pointers to backend server addrinfo objects
    */
    addrinfo *backend_A_addr;
    addrinfo *backend_B_addr;

    /**
     * unordered map with department names as keys and their corresponding backend
     * server IDs as values
    */
    servmap serverdata;

    /**
     * fill server_tcp_info with info of server to be used to construct a TCP socket
     * @param server_tcp_info reference to a blank addrinfo struct to be filled with
     * server info
     * @return  boolean indicating if this operation is successful
    */
    bool get_tcp_addr(struct addrinfo *& server_tcp_info);

    /**
     * fill udp_info with info of server to be used to construct a UDP socket
     * @param udp_info reference to a blank addrinfo struct to be filled with
     * server info
     * @param port_number port number to be used for addrinfo object
     * @return  boolean indicating if this operation is successful
    */
    bool get_udp_addr(struct addrinfo *& udp_info, const char *port_number);

    /**
     * create a server socket based on server_info, store socket in socket_fd
     * @param server_info addrinfo struct with info of server
     * @param socket_fd file descriptor of the socket to be constructed
     * @return  boolean indicating if this operation is successful
    */
    bool create_socket(const struct addrinfo *server_info, int& socket_fd);

    /**
     * bind this->sockfd to address specified in server_info
     * @param server_info addrinfo struct with info of server
     * @param socket_fd file descriptor of the socket
     * @return  boolean indicating if this operation is successful
    */
    bool bind_socket(const struct addrinfo *server_info, int& socket_fd);

    /**
     * start listening for incoming connection on this->sockfd
     * @return  boolean indicating if this operation is successful
    */
    bool start_listen();

    /**
     * fill in this->serverdata with each department in dep_list
     * @param backend_id  id of backend server
     * @param dep_list  list of departments on this backend server
    */
    void generate_serverdata(const std::string& backend_id, std::string& dep_list);

    /**
     * request a department list from backend server
     * @param backend_id  id of backend server
     * @param dep_list  list of departments on this backend server
    */
    void request_dep_list(const std::string& backend_id, std::string& dep_list);

    /**
     * accept a client connection and set client_sockfd
     * @param client_sockfdptr  pointer to file descriptor of socket dedicated for
     * a client
     * @return  boolean indicating if this operation is successful
    */
    bool accept_client_connection(int *client_sockfdptr);

    /**
     * receive query from client
     * @param client_sockfd file descriptor for socket for client connection
     * @param client_id port number of client that sent the query
     * @param dep_name  department name sent by client as part of query
     * @param stud_id student id sent by client as part of query
     * @return  boolean indicating if query is successfully received
    */
    bool recv_query(const int client_sockfd, const int client_id, std::string& dep_name, std::string& stud_id);

    /**
     * forward client query to backend server that contains the department
     * @param backend_id id of backend server that the query is forwarded to
     * @param dep_name department name sent by client as part of query
     * @param stud_id student id sent by client as part of query
     * @return  boolean indicating if query is successfully received
    */
    bool send_query(const std::string& backend_id, std::string& dep_name, std::string& stud_id);

    /**
     * receive response from backend server to the client query
     * @param backend_id id of backend server that the query is forwarded to
     * @param stud_id student id sent by client as part of query
     * @param response  response sent by backend server
     * @param flag  int indicating status of query, 0 for normal, 1 for department 
     * name not found, and 2 for student id not found
    */
    void recv_response(const std::string& backend_id, const std::string& stud_id, std::string& response, int& flag);

    /**
     * forward to client response from the backend to client's query
     * @param client_sockfd file descriptor for socket for client connection
     * @param client_id reference to client id string
     * @param message response to be sent to client
     * @param flag  int indicating status of query, 0 for normal, 1 for department 
     * name not found, and 2 for student id not found
    */
    void send_response(const int client_sockfd, const int client_id, const std::string& message, const int flag);

    /**
     * handle client query
     * @param client_sockfd file descriptor for socket for client connection
     * @param client_id reference to client id string
     * @return  boolean indicating if server should maintain connection with client
    */
    bool respond_to_client(const int client_sockfd, const int client_id);
};
