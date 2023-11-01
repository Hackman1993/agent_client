/***********************************************************************************************************************
 * @author Hackman Lo
 * @file global_status.cpp
 * @description 
 * @created_at 2023-10-18
***********************************************************************************************************************/

#include "global_status.h"
#include <boost/dll.hpp>
#include <sahara/utils/uuid.h>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
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

std::uint16_t global_status::server_http_port() {
    return check_ptr()->server_http_port_;
}

std::uint16_t global_status::server_dispatch_port() {
    return check_ptr()->server_dispatch_port_;
}

global_status::global_status() {
    program_path_ = boost::dll::program_location().remove_filename().string();

    std::filesystem::path config_path;
    config_path.append(program_path_.string()).append("config.json");
    boost::property_tree::ptree ptree;
    bool readed = false;
    if(std::filesystem::exists(config_path)){
        try{
            boost::property_tree::json_parser::read_json(config_path.string(), ptree);
            readed = true;
            client_id_ = ptree.get<std::string>("client_id");
        }catch (boost::property_tree::json_parser_error& e){
            std::cout << e.what()<< std::endl;
        }
    }

    if(!readed){
        try {
            client_id_ = sahara::utils::uuid::generate();
            ptree.add("client_id", client_id_);
            boost::property_tree::json_parser::write_json(config_path.string(), ptree);
        }catch (boost::property_tree::json_parser_error& e){
            std::cout << e.what() << std::endl;
        }
    }
}

std::filesystem::path global_status::system_path() {
    return check_ptr()->program_path_;
}

bool global_status::extract_command_option(int argc, char **argv) {
    return check_ptr()->extract_command_option_(argc, argv);
}

bool global_status::extract_command_option_(int argc, char **argv) {
    boost::program_options::options_description options("all options");
    boost::program_options::variables_map vm_map;
    options.add_options()
            ("host,H", boost::program_options::value<std::string>()->default_value("127.0.0.1"))
            ("serv_port,p", boost::program_options::value<std::uint16_t>()->default_value(3308))
            ("http_port,t", boost::program_options::value<std::uint16_t>()->default_value(3309));
    try{
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), vm_map);
    }catch (std::exception& e){
        std::cout << e.what() << std::endl;
        return false;
    }
    if(vm_map.count("host")){
        server_ip_ = vm_map["host"].as<std::string>();
    }
    if(vm_map.count("serv_port")) {
        server_dispatch_port_ = vm_map["serv_port"].as<std::uint16_t>();
    }
    if(vm_map.count("http_port")){
        server_http_port_ = vm_map["http_port"].as<std::uint16_t>();
    }
    return true;
}
