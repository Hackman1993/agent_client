/***********************************************************************************************************************
 * @author Hackman Lo
 * @file server_connection.cpp
 * @description 
 * @created_at 2023-10-18
***********************************************************************************************************************/

#include "server_connection.h"
#include "agent/packages/protos/system.pb.h"
#include "global/version.h"
#include "global/global_status.h"
#include "agent/global/ability_define.h"
#include "job/sync_job.h"
#include "job/job_manager.h"

void server_connection::e_connected() {
    package_header header;
    agent::package::client::client_info info;
    info.set_client_id_(global_status::client_id());
    info.set_ability_(AGENT_ABILITY_MASK);
#ifdef _WIN32
    info.set_os_(agent::package::client::OS_TYPE_WINDOWS);
#elif defined(__linux__)
    info.set_os_(agent::package::client::OS_TYPE_LINUX);
#elif defined(__MACH__)
    info.set_os_(agent::package::client::OS_TYPE_MACOS);
#else
    info.set_os_(agent::package::client::OS_TYPE_UNIX);
#endif
    info.set_client_version_(AGENT_VERSION);
    header.operation_ = PACKAGE_AGENT_REGISTER;
    package<agent::package::client::client_info> package(PACKAGE_AGENT_REGISTER, info);
    std::cout << package.size() << std::endl;
    send_package(package);
}

server_connection::server_connection(boost::asio::io_context &ios_) : agent_connection(ios_), timer_(ios) {
    on_package(PACKAGE_AGENT_DUPLICATED_ID, [&](const std::shared_ptr<agent_connection>& connection, std::string_view data){
        this->on_duplicated_client_id(std::dynamic_pointer_cast<server_connection>(connection), data);
    });
    on_package(PACKAGE_RENDER_JOB_DISPATCH, [&](const std::shared_ptr<agent_connection>& connection, std::string_view data){
        this->on_job_data_received(std::dynamic_pointer_cast<server_connection>(connection), data);
    });
}

void server_connection::on_duplicated_client_id(std::shared_ptr<server_connection> connection,std::string_view data) {
    std::cout << "Duplicate Client! Server rejected!" << std::endl;
    this->close();
}

void server_connection::on_job_data_received(std::shared_ptr<server_connection> connection, std::string_view data) {
    agent::package::renderer::rendering_job job;
    auto result = job.ParseFromArray(data.data(), data.size());
    if(!result){
        std::cout << "Job Data Parse Failed" << std::endl;
        return;
    }

    auto job_item = std::make_shared<sync_job>(job);
    job_manager::add_job(job_item);

}

