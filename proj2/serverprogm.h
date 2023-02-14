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
    /**
     * port number of server
    */
    static const char *SERVER_PORT;

    /**
     * name of text data file to be read
    */
    static const char *DATAFILE;

    /**
     * number of simultaneous connections to the listening socket
    */
    static const int BACKLOG;

    /**
     * size of buffer to receive department name sent by client
    */
    static const int BUFFER_SIZE;

    /**
     * size of buffer to receive client id
    */
    static const int ID_BUFFER_SIZE;

    /**
     * set up server socket, read data file, and listen for incoming connections
     * @return  a boolean indicating if the server socket is set up correctly
    */
    bool setup_server();

    /**
     * accept client connection and respond to client query in child process
    */
    void run_server();

  private:
    /**
     * file descriptor for main server socket
    */
    int sockfd;

    /**
     * vector of strings representing unique backend server IDs, in the order
     * they are written in data file
    */
    strvec serv_nums;

    /**
     * unordered map with department names as keys and their corresponding backend
     * server IDs as values
    */
    servmap serverdata;

    /**
     * read inputs; if input line break is in Windows style (\r\n), remove \r
     * character from the line
     * @param input lines of strings as input
     * @param line  the string object to which one line of input will be read into
    */
    void getline_no_cr(std::istream& input, std::string& line);

    /**
     * print out all backend server IDs and their corresponding number of distinct
     * department names on those servers
     * uses this->serv_nums
     * @param distinct_dep_nums vector containing number of distinct department names
     * on each backend server, in the same order as servers in this->serv_nums
    */
    void print_list_file(intvec& distinct_dep_nums);

    /**
     * read input file two lines at a time; 1st line is server ID, 2nd line is
     * department names; store result in this->serverdata and this->serv_nums
     * @param input input file as istream
    */
    void read_file(std::istream& input);

    /**
     * fill server_info with info of server to be used to construct a socket
     * @param server_info reference to a blank addrinfo struct to be filled with
     * server info
     * @return  boolean indicating if this operation is successful
    */
    bool get_addr(struct addrinfo *& server_info);

    /**
     * create a server socket based on server_info, store socket in this->sockfd
     * @param server_info addrinfo struct with info of server
     * @return  boolean indicating if this operation is successful
    */
    bool create_socket(const struct addrinfo *server_info);

    /**
     * bind this->sockfd to address specified in server_info
     * @param server_info addrinfo struct with info of server
     * @return  boolean indicating if this operation is successful
    */
    bool bind_socket(const struct addrinfo *server_info);

    /**
     * start listening for incoming connection on this->sockfd
     * @return  boolean indicating if this operation is successful
    */
    bool start_listen();

    /**
     * accept a client connection and set client_sockfd
     * @param client_sockfdptr  pointer to file descriptor of socket dedicated for
     * a client
     * @return  boolean indicating if this operation is successful
    */
    bool accept_client_connection(int *client_sockfdptr);

    /**
     * receive client id from client
     * @param client_sockfd file descriptor for socket for client connection
     * @return  client id as string, or empty string if there's an error
    */
    std::string recv_client_id(const int client_sockfd);

    /**
     * receive department name as query from client
     * @param client_sockfd file descriptor for socket for client connection
     * @param client_id reference to client id string
     * @return  the received department name as string, or empty string if there's
     * an error or if client disconnects
    */
    std::string recv_dep_name(const int client_sockfd, const std::string &client_id);

    /**
     * send to client the backend server ID corresponding to client's query, or 
     * "-1" if no such server
     * @param client_sockfd file descriptor for socket for client connection
     * @param serv_num  reference to backend server ID string
     * @param client_id reference to client id string
    */
    void send_serv_num(const int client_sockfd, const std::string& serv_num, const std::string &client_id);

    /**
     * handle client query for server ID
     * receive department name by this->recv_dep_name(), check if client is still
     * connected, perform lookup in this->serverdata for department name, and send
     * back to client the requested server ID (or null if no such server)
     * @param client_sockfd file descriptor for socket for client connection
     * @param client_id reference to client id string
     * @return  boolean indicating if server should maintain connection with client
    */
    bool respond_to_client(const int client_sockfd, const std::string &client_id);
};
