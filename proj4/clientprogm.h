// struct addrinfo
// In cpp file:
// getaddrinfo(), freeaddrinfo(), gai_strerror()
#include <netdb.h>
// string, to_string()
#include <string>


class ClientProgm {
  public:
    /**
     * IP address of server
    */
    static const char *SERVER_IP;

    /**
     * port number of server
    */
    static const char *SERVER_PORT;

    /**
     * size of buffer to receive response sent by server
    */
    static const int BUFFER_SIZE;

    /**
     * set up client socket, connect to server
     * @return boolean indicating if client socket is set up correctly
    */
    bool setup_client();

    /**
     * keep sending user input query to server and receive response
    */
    void run_client();

  private:
    /**
     * file descriptor for client socket
    */
    int sockfd;

    /**
     * port number of client
    */
    std::string client_port;

    /**
     * read user input
     * @return  user input as string
    */
    std::string read_input(std::istream& input);

    /**
     * fill server_info with info of server to be used to construct a socket
     * @param server_info reference to a blank addrinfo struct to be filled 
     * with server info
     * @return  boolean indicating if this operation is successful
    */
    bool get_addr(struct addrinfo *& server_info);

    /**
     * create a client socket based on server_info, store socket in 
     * this->sockfd
     * @param server_info addrinfo struct with info of server
     * @return  boolean indicating if this operation is successful
    */
    bool create_socket(const struct addrinfo *server_info);

    /**
     * connect to server with client socket
     * @param server_info addrinfo struct with info of server
    */
    bool connect_to_server(const struct addrinfo *server_info);

    /**
     * get client port number by getsockname()
     * @return  boolean indicating if this operation is successful
    */
    bool get_client_id();

    /**
     * send department name and student id as query to server
     * @param dep_name  department name as part of query
     * @param stud_id stuent id as part of query
     * @return  boolean indicating if this operation is successful
    */
    bool send_query(const std::string& dep_name, const std::string& stud_id);

    /**
     * receive query response from server
     * @param dep_name  department name as part of query
     * @param stud_id stuent id as part of query
    */
    void recv_response(const std::string& dep_name, const std::string& stud_id);

    /**
     * handle query sending and receiving
     * send query to server, and if this is successful, receive response
     * @param dep_name  department name as part of query
     * @param stud_id stuent id as part of query
     * @return  boolean indicating if this operation is successful
    */
    bool request_from_server(const std::string& dep_name, const std::string& stud_id);
};
