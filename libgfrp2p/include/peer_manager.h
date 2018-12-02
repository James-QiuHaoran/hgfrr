#include <string>
#include <unordered_set>
#include "node.h"
#include "app.h"

/* Message class
 * definition of messages transmitted among peers
 */
class Message {
private:
    std::string sender;
    std::string receiver;
    std::string messageID;
    std::string messageHash;
    std::string type;

public:
    // constructor
    Message();

    // getters
    std::string get_sender() const;
    std::string get_receiver() const;
    std::string get_messageID() const;
    std::string get_messageHash() const;
    std::string get_type() const;

    // setters
    void set_sender(string sender);
    void set_receiver(string receiver);
    void set_messageID(string messageId);
    void set_messageHash(string messageHash);
    void set_type(string type);
};

/* Error Class
 * define error types and error messages
 *
 */
class PeerError {
private:
    std::string errorType;
    std::string errorMessage;

public:
    // constructor
    PeerError();
    
    // getters
    std::string get_errorType();
    std::string get_errorMessage();

    // setters
    void set_errorType();
    void set_errorMessage();
};

/* PeerManager class
 * responsible for broadcasting messages
 */
class PeerManager {
private:
    // peers[]
    // listen address
    // server

    std::unordered_set<Node> contact_nodes_this;
    std::unordered_set<Node> contact_nodes_upper;

public:
    // constructor
    void PeerManager();

    // getters
    std::unordered_set<Node> get_contact_nodes_this();
    std::unordered_set<Node> get_contact_nodes_upper();
    
    // create and initialize a peer
    void create_peer();

    // connect to the network
    void connect();

    // start the server
    void start();

    // broadcast a message
    void broadcast(Message msg);
    void broadcast_up(Message msg);
    void broadcast_down(Message msg);

    // on joining a node
    void on_new_connection();

    // detect node leave
    void on_node_lost_connection();
    void on_node_leave();

    // stop the peer
    void stop();
};