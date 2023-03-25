// ifstream
#include <fstream>
// struct addrinfo
// In cpp file:
// getaddrinfo(), freeaddrinfo(), gai_strerror()
#include <netdb.h>
// string
// In cpp file:
// size_t, 
// getline(), length(), rbegin(), erase(), to_string(), find(), substr()
#include <string>
// ssize_t
// In cpp file:
// AF_INET, SOCK_DGRAM, struct sockaddr_storage, socklen_t,
// bind(), connect(), socket(), recv(), send()
#include <sys/socket.h>
// unordered_map
// In cpp file:
// emplace(), end(), find()
#include <unordered_map>


typedef std::unordered_map<std::string, std::string> servmap;

class BackendProg {
  public:
    /**
     * ip of backend and main server
    */
    static const char *HOST_IP;

    /**
     * size of buffer to receive department name sent by main server
    */
    static const int BUFFER_SIZE;

    /**
     * constructor
    */
    BackendProg(
      const std::string backend_id, 
      const char *filename, 
      const char *backend_port, 
      const char *server_port
    );

    /**
     * set up backend socket, read data file, and send department list
     * @return  a boolean indicating if the socket is set up correctly
    */
    bool setup_server();

    /**
     * respond to main server query
    */
    void run_server();

  private:
    /**
     * file descriptor for backend socket
    */
    int sockfd;

    /**
     * id of current backend server ("A" or "B")
    */
    std::string backend_id;

    /**
     * name of data file to be read by backend ("dataA.txt" or "dataB.txt")
    */
    const char *filename;

    /**
     * port number of this backend server and the main server
    */
    const char *backend_port;
    const char *server_port;

    /**
     * unordered map with department names as keys and unique student IDs in
     * this department as values
    */
    servmap serverdata;

    /**
     * read inputs; if input line break is in Windows style (\r\n), remove \r
     * character from the line
     * @param input lines of strings as input
     * @param line  string object to which one line of input will be read into
    */
    void getline_no_cr(std::istream& input, std::string& line);

    /**
     * extract from a line of text all the unique student IDs, store information
     * as a ", "-separated string
     * @param line  line of text from input file
     * @return  string containing unique IDs as a ", "-separated string, with a 
     * trailing ", "
    */
    std::string extract_unique_ids(const std::string &line);

    /**
     * read input two lines at a time; 1st line is department name, 2nd 
     * line is student IDs; store result in this->serverdata
     * @param input input as istream
    */
    void read_file(std::istream& input);

    /**
     * fill addr_info with info of host
     * @param addr_info reference to a blank addrinfo struct to be filled with
     * info of desired address
     * @param port_number the port number to be used for this addr_info
     * @return  boolean indicating if this operation is successful
    */
    bool get_addr(struct addrinfo *& addr_info, const char *port_number);

    /**
     * create a UDP socket based on backend_info, store socket in this->sockfd
     * @param backend_info  addrinfo struct with info of server
     * @return  boolean indicating if this operation is successful
    */
    bool create_socket(const struct addrinfo *backend_info);

    /**
     * bind this->sockfd to address specified in backend_info
     * @param backend_info  addrinfo struct with info of backend server
     * @return  boolean indicating if this operation is successful
    */
    bool bind_socket(const struct addrinfo *backend_info);

    /**
     * "connect" to another UDP socket so that address info will be auto filled
     * when send() and recv()
     * @param server_info  addrinfo containing address of the other socket
     * @return  boolean indicating if this operation is successful
    */
    bool UDP_connect(const struct addrinfo *server_info);

    /**
     * send to main server a list of departments stored on this backend server
    */
    void send_dep_list(const std::string &dep_list);

    /**
     * receive department name as query from main server
     * @return  the received department name as string, or empty string if there's
     * an error or if client disconnects
    */
    std::string recv_dep_name();

    /**
     * send to main server the student IDs corresponding to the query
     * @param stud_id_info  a ", "-separated list of distinct student IDs, followed
     * by the number of these IDs, separated from the IDs by a period
    */
    void send_stud_ids(const std::string &stud_id_info);

    /**
     * handle main server query for server ID
     * receive department name by this->recv_dep_name(), perform lookup in 
     * this->serverdata for department name, and send back to main server the 
     * requested student IDs
     * @return  boolean indicating if this operation is successful
    */
   
    bool respond_to_query();
};
