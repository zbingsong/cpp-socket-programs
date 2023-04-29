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
// map
#include <map>
// vector
#include <vector>


// types for private instance variable
typedef std::unordered_map<std::string, std::string> depstatsmap;
typedef std::unordered_map<std::string, std::string> depstudmap;
typedef std::unordered_map<int, std::string> recmap;
// types for temporary variables
typedef std::map<float, std::vector<int>> scoremap;
typedef std::unordered_map<std::string, scoremap> depscoremap;
typedef std::unordered_map<std::string, float> rankmap;
typedef std::unordered_map<int, rankmap> studrankmap;
typedef std::unordered_map<std::string, int> depsizemap;


class BackendProg {
  public:
    /**
     * ip and port of backend server
    */
    static const char *HOST_IP;
    static const char *SERVER_PORT;

    /**
     * size of buffer to receive query sent by main server
    */
    static const int BUFFER_SIZE;

    /**
     * constructor
    */
    BackendProg(
      const std::string backend_id, 
      const char *filename, 
      const char *backend_port
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
     * name of data file to be read by backend ("dataA.csv" or "dataB.csv")
    */
    const char *filename;

    /**
     * port number of this backend server
    */
    const char *backend_port;

    /**
     * unordered map of department names to generated department
     * statistics as strings
    */
    depstatsmap dep_gpa;

    /**
     * unordered map of "<department name>,<student id>" to student's GPA 
     * and percentage rank as string
    */
    depstudmap dep_stud_stats;

    /**
     * unordered map of student id (int) to id of recommended student (int)
    */
    recmap recommendations;

    /**
     * compute student's gpa and put it into a scoremap of some department
     * @param stud_scores a vector of student's scores in a department
     * @param stud_id student's id
     * @param score_map a map of gpa to a vector of students who have this gpa
    */
    void process_single_gpa(const std::vector<int>& stud_scores, const int stud_id, scoremap& score_map);

    /**
     * add dep_name as a key in dep_score_map if not already, and increment 
     * size of department
     * @param dep_name  name of department
     * @param dep_score_map map of department names to scoremaps
     * @param dep_size  map of department names to number of students in that
     * department
    */
    void process_dep_name_token(const std::string& dep_name, depscoremap& dep_score_map, depsizemap& dep_size);

    /**
     * tokenize a line in input file and process department name, student id, and
     * student scores
     * @param line  a line in the input file
     * @param dep_score_map map of department names to scoremaps
     * @param dep_size map of department names to number of students in that 
     * department
    */
    void process_line_input(std::string& line, depscoremap& dep_score_map, depsizemap& dep_size);

    /**
     * get the min, max, average, and variance of GPA of a department, and store
     * the result as a string in this->dep_gpa
     * @param dep_name deparment name
     * @param score_map map of gpa to vector of students who have that gpa
     * @param dep_total sum of gpa of all students in that department
     * @param num_stud  number of students in the department
    */
    void generate_dep_stats(const std::string& dep_name, const scoremap& score_map, const float& dep_total, const int& num_stud);

    /**
     * get a recommendation for each student
     * @param stud_ranks  map of student id to (map of department names to 
     * the student's percentage rank in that department)
    */
    void generate_recommendations(const studrankmap& stud_ranks);

    /**
     * get all statistics needed to answer queries and store them in private
     * instance variables
     * @param dep_score_map map of department names to scoremaps
     * @param dep_size  map of department names to number of students in that 
     * department
    */
    void generate_server_data(const depscoremap& dep_score_map, const depsizemap& dep_size);

    /**
     * read input file and store computed statistics in memory, send department
     * list to main server
     * @param input input file as istream
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
     * @param dep_list  a list of departments on this server
    */
    void send_dep_list(const std::string& dep_list);

    /**
     * receive department name and student id as query from main server
     * @param dep_name  department name received
     * @param stud_id student id received
    */
    void recv_query(std::string& dep_name, int& stud_id);

    /**
     * send to main server the statistics corresponding to the query, orr
     * a message indicating the student id was not found
     * @param response  string response to be sent to main server
    */
    void send_response(const std::string& response);

    /**
     * handle main server query
     * receive query by this->recv_query(), perform lookup, and send back
     * to main server the requested statistics by this->send_response()
     * @return  boolean indicating if this operation is successful
    */
    bool respond_to_query();
};
