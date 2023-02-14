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
     * size of buffer to receive server ID sent by server
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
     * read user input of department names
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
     * get client port number by getsockname(), and send it as client id to 
     * server
     * @return  boolean indicating if this operation is successful
    */
    bool send_client_id();

    /**
     * send department name as query to server
     * @param dep_name  reference to department name string
     * @return  boolean indicating if this operation is successful
    */
    bool send_dep_name(const std::string& dep_name);

    /**
     * receive server ID corrsponding to department name from server, or "-1" 
     * if no such server; print out the result
     * @param dep_name  reference to department name string
    */
    void recv_serv_num(const std::string& dep_name);

    /**
     * handle query sending and receiving
     * send department name to server, and if this is successful, receive
     * server response
     * @param dep_name  reference to department name string
    */
    bool request_from_server(const std::string& dep_name);
};
