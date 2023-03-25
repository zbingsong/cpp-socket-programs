// struct addrinfo
// In cpp file:
// getaddrinfo(), freeaddrinfo(), gai_strerror()
#include <netdb.h>
// string
#include <string>
// unordered_map, emplace()
// In cpp file:
// cend()
#include <unordered_map>
// vector, push_back()
#include <vector>


typedef std::vector<std::string> strvec;
typedef std::unordered_map<std::string, std::string> servmap;

class ServerMainProgm {
  public:
    /**
     * IP address of server
    */
    static const char *HOST_IP;

    /**
     * buffer size for getting department list
    */
    static const int DEP_LIST_BUFFER_SIZE;

    /**
     * size of buffer to receive student IDs
    */
    static const int BUFFER_SIZE;

    /**
     * constructor
     * @param server_port port of this main server
     * @param backend_A_port  port of backend server A
     * @param backend_B_port  port of backend server B
    */
    ServerMainProgm(
      const char *server_port,
      const char *backend_A_port,
      const char *backend_B_port
    );

    /**
     * set up main server socket and fill in addrinfo of backend servers
     * @return boolean indicating if the socket is set up correctly
    */
    bool setup_server();

    /**
     * keep sending user query to backend and receive response
    */
    void run_server();

  private:
    /**
     * file descriptor for client socket
    */
    int sockfd;

    /**
     * port numbers of this main server and two backend servers
    */
    const char *server_port;
    const char *backend_A_port;
    const char *backend_B_port;

    /**
     * addrinfo of backend servers, used for sending messages thru UDP
    */
    struct addrinfo *backend_A_addr;
    struct addrinfo *backend_B_addr;

    /**
     * map of department name to backend server ID
    */
    servmap serverdata;

    /**
     * read user input of department names
     * @param input user input as istream
     * @return  user input as string
    */
    std::string read_input(std::istream &input);

    /**
     * fill addr_info with info of desired address
     * @param addr_info reference to a blank addrinfo struct to be filled
     * @param port_number port_number to be used for this address
     * @return  boolean indicating if this operation is successful
    */
    bool get_addr(struct addrinfo *& addr_info, const char *port_number);

    /**
     * create a UDP socket based on server_info, store socket in this->sockfd
     * @param server_info addrinfo struct with info of server
     * @return  boolean indicating if this operation is successful
    */
    bool create_socket(const struct addrinfo *server_info);

    /**
     * bind this->sockfd to address specified in server_info
     * @param server_info  addrinfo struct with info of main server
     * @return  boolean indicating if this operation is successful
    */
    bool bind_socket(const struct addrinfo *server_info);

    /**
     * get a list of departments on the backend server specified by backend_id,
     * store result in this->serverdata
     * @param backend_id  ID of the backend server to receive list from
    */
    strvec request_dep_list(const std::string &backend_id);

    /**
     * send department name as query to server
     * @param dep_name  reference to department name string
     * @param backend_id  ID of the backend server to receive IDs from
     * @return  boolean indicating if this operation is successful
    */
    bool send_dep_name(const std::string &dep_name, const std::string &backend_id);

    /**
     * receive student IDs corrsponding to department name from backend
     * @param dep_name  reference to department name string
     * @param backend_id  ID of the backend server to receive IDs from
    */
    void recv_stud_ids(const std::string &dep_name, const std::string &backend_id);

    /**
     * handle query sending and receiving
     * send department name to backend, and if this is successful, receive
     * student IDs as response
     * @param dep_name  reference to department name string
     * @return  boolean indicating if query was successfully sent and answered
    */
    bool query_for_dep(const std::string &dep_name);
};
