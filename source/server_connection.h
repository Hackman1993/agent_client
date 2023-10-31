/***********************************************************************************************************************
 * @author Hackman Lo
 * @file server_connection.h
 * @description 
 * @created_at 2023-10-18
***********************************************************************************************************************/

#ifndef AGENT_SERVER_CONNECTION_H
#define AGENT_SERVER_CONNECTION_H
#include "obelisk/obelisk.h"
#include <thread>
#include "agent/agent_connection.h"
#include "agent/packages/package.h"
#include "agent/packages/protos/rendering_job.pb.h"
class server_connection: public agent_connection {
public:
    server_connection(boost::asio::io_context& ios);


protected:
    std::unordered_map<std::string, bool> syncing_projects_;

    std::shared_ptr<agent::package::renderer::rendering_job> current_job_;
    std::atomic_uint32_t current_status_ = 0; // 0: Not Doing Anything, 1 Waiting for sync files, 2 Rendering
    boost::asio::deadline_timer timer_;

    void e_connected() override;
    void on_duplicated_client_id(std::shared_ptr<server_connection> connection, std::string_view data);
    void on_job_data_received(std::shared_ptr<server_connection> connection, std::string_view data);
};


#endif //AGENT_SERVER_CONNECTION_H
