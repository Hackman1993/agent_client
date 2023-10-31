/***********************************************************************************************************************
 * @author Hackman Lo
 * @file global_status.h
 * @description 
 * @created_at 2023-10-18
***********************************************************************************************************************/

#ifndef AGENT_GLOBAL_STATUS_H
#define AGENT_GLOBAL_STATUS_H
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include "agent/packages/protos/rendering_job.pb.h"
#include "server_connection.h"

class global_status {
public:
    static const std::string& client_id();
    static bool contains_project(const std::string& project_id);
    static agent::package::renderer::render_project project(const std::string& project_id);
    static void add_project(const std::string& project_id, agent::package::renderer::render_project data);
    static void remove_project(const std::string& project_id);

    static const std::string& server_ip();
    static const std::uint16_t server_http_port();
    static const std::uint16_t server_dispatch_port();

private:
    std::string client_id_ = "123";
    std::string server_ip_ = "192.168.0.191";
    std::uint16_t server_http_port_ = 3309;
    std::uint16_t server_dispatch_port_ = 3308;
    global_status() = default;
    static const std::unique_ptr<global_status>& check_ptr();
    static std::unique_ptr<global_status> self_ptr_;

    std::mutex project_mutex_;
    std::unordered_map<std::string, agent::package::renderer::render_project> project_data_;
};

extern boost::asio::io_context ios;
extern std::shared_ptr<server_connection> connection;

#endif //AGENT_GLOBAL_STATUS_H
