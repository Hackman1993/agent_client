/***********************************************************************************************************************
 * @author Hackman Lo
 * @file global_status.cpp
 * @description 
 * @created_at 2023-10-18
***********************************************************************************************************************/

#include "global_status.h"
std::unique_ptr<global_status> global_status::self_ptr_ = nullptr;
const std::unique_ptr<global_status> &global_status::check_ptr() {
    if(!self_ptr_)
        self_ptr_ = std::unique_ptr<global_status>(new global_status());
    return self_ptr_;
}

const std::string &global_status::client_id() {
    return check_ptr()->client_id_;
}

bool global_status::contains_project(const std::string &project_id) {
    std::scoped_lock<std::mutex> lock(check_ptr()->project_mutex_);
    return check_ptr()->project_data_.contains(project_id);
}

agent::package::renderer::render_project global_status::project(const std::string &project_id) {
    std::scoped_lock<std::mutex> lock(check_ptr()->project_mutex_);
    return check_ptr()->project_data_[project_id];
}

void global_status::remove_project(const std::string &project_id) {
    std::scoped_lock<std::mutex> lock(check_ptr()->project_mutex_);
    check_ptr()->project_data_.erase(project_id);
}

void global_status::add_project(const std::string &project_id, agent::package::renderer::render_project data) {
    std::scoped_lock<std::mutex> lock(check_ptr()->project_mutex_);
    check_ptr()->project_data_.emplace(project_id, data);
}

const std::string &global_status::server_ip() {
    return check_ptr()->server_ip_;
}

const std::uint16_t global_status::server_http_port() {
    return check_ptr()->server_http_port_;
}

const std::uint16_t global_status::server_dispatch_port() {
    return check_ptr()->server_dispatch_port_;
}
