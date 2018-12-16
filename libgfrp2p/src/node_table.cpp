#include "node_table.h"

/* private functions */
std::shared_ptr<Node> NodeTable::get_node(unsigned long level, const std::string& id) {
    auto ring = tables[level];

    // find from contact nodes
    auto contact_nodes_iter = ring.contact_nodes.find(id);
    if (contact_nodes_iter != ring.contact_nodes.end()) 
        return contact_nodes_iter->second;
    
    // find from peer list
    auto peer_list_iter = ring.peer_list.find(id);
    if (peer_list_iter != ring.peer_list.end())
        return peer_list_iter->second;
    
    // not found
    return std::shared_ptr<Node>();
}

std::shared_ptr<Node> NodeTable::copy_node(const std::shared_ptr<Node>& node) {
    return std::make_shared<Node>(*node);
}

/* public functions */
NodeTable::NodeTable(const std::string& self_id):
    self_id(self_id), self_level(0) { }

std::string NodeTable::get_self_id() const {
    return this->self_id;
}

bool NodeTable::has_node(unsigned long level, const std::string& id) {
    std::lock_guard<std::mutex> lock(this->mlock);
    auto ring = this->tables[level];
    if (ring.contact_nodes.find(id) != ring.contact_nodes.end() || ring.peer_list.find(id) != ring.peer_list.end())
        return true;
    return false;
}

std::shared_ptr<Node> NodeTable::get_node_copy(unsigned long level, const std::string& id) {
    std::lock_guard<std::mutex> lock(this->mlock);
    auto node = this->get_node(level, id);
    if (node)
        return this->copy_node(node);
    return std::shared_ptr<Node>();
}

void NodeTable::set_node_last_ping_now(unsigned long level, const std::string& id) {
    std::lock_guard<std::mutex> lock(this->mlock);
    auto node = this->get_node(level, id);
    if (!node) return;
    node->set_last_ping_now();
}

void NodeTable::set_node_last_pong_now(unsigned long level, const std::string& id) {
    std::lock_guard<std::mutex> lock(this->mlock);
    auto node = this->get_node(level, id);
    if (!node) return;
    node->set_last_pong_now();
}

/* domain logic functions */
bool NodeTable::is_contact_node(unsigned long level) {
    std::lock_guard<std::mutex> lock(this->mlock);
    if (level > this->tables.size() - 1) {
        return false;  // does not resides in the ring of that level
    } else if (level < this->tables.size() - 1) {
        return true;   // resides in higher levels -> must be one of the contact nodes of that level's ring
    } else {
        return false;  // does not resides in one level higher
    }
}

std::unordered_set<std::shared_ptr<Node>> NodeTable::get_contact_nodes(unsigned long level) {
    std::lock_guard<std::mutex> lock(this->mlock);
    std::unordered_set<std::shared_ptr<Node>> result;

    // does not reside in that level's ring
    if (level > this->tables.size() - 1)
        return result;

    // return contact nodes of the ring
    auto ring = this->tables.at(level);
    for (const auto& kv: ring.contact_nodes) {
        result.insert(this->copy_node(kv.second));
    }  
    return result;
}

std::shared_ptr<Node> NodeTable::get_successor(unsigned long level) {
    std::lock_guard<std::mutex> lock(this->mlock);

    // does not reside in that level's ring
    if (level > this->tables.size() - 1)
        return result;

    // return contact nodes of the ring
    auto ring = this->tables.at(level);

    std::shared_ptr<Node> result = this->copy_node(ring.successor);
    return result;
}

std::unordered_set<std::shared_ptr<Node>> NodeTable::get_predecessor(unsigned long level) {
    std::lock_guard<std::mutex> lock(this->mlock);
    
    // does not reside in that level's ring
    if (level > this->tables.size() - 1)
        return result;

    // return contact nodes of the ring
    auto ring = this->tables.at(level);

    std::shared_ptr<Node> result = this->copy_node(ring.predecessor);
    return result;
}

std::unordered_set<std::shared_ptr<Node>> NodeTable::get_peer_list(unsigned long level) {
    std::lock_guard<std::mutex> lock(this->mlock);
    std::unordered_set<std::shared_ptr<Node>> result;

    // does not reside in that level's ring
    if (level > this->tables.size() - 1)
        return result;

    // return contact nodes of the ring
    auto ring = this->tables.at(level);
    for (const auto& kv: ring.peer_list) {
        result.insert(this->copy_node(kv.second));
    }  
    return result;
}

std::shared_ptr<Node> NodeTable::get_peer(unsigned long level,  const std::string& id) {
    // does not reside in that level's ring
    if (level > this->tables.size() - 1)
        return std::shared_ptr<Node>();

    // return the particular node
    return this->get_node_copy(level, id);
}

int NodeTable::get_end_id(unsigned long level) {
    // does not reside in that level's ring
    if (level > this->tables.size() - 1)
        return -1;

    // return the end id
    auto ring = this->tables.at(level);
    return ring.peer_list.size();
}