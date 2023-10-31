#include <iostream>
#include <boost/asio.hpp>
#include "global/global_status.h"
#include "server_connection.h"
boost::asio::io_context ios;
std::shared_ptr<server_connection> connection = std::make_shared<server_connection>(ios);
int main() {
    agent::package::renderer::render_project project;
    project.set_project_id("test_project");
    project.set_begin_frame(1);
    project.set_end_frame(90);
    project.set_blender_version("3.6");
    project.mutable_blend_file()->set_relative_path("untitled.blend");
    project.mutable_blend_file()->set_download_url("http://192.168.0.191:3309/test_project/untitled.blend");
    auto file = project.add_files();
    file->set_download_url("http://192.168.0.191:3309/test_project/SD-002.IES");
    file->set_relative_path("SD-002.IES");
    file = project.add_files();
    file->set_relative_path("untitled.blend");
    file->set_download_url("http://192.168.0.191:3309/test_project/untitled.blend");
    package<agent::package::renderer::render_project> pkg(PACKAGE_NEW_RENDER_PROJECT, project);
    auto data = pkg.data();
    for(int i =0; i < pkg.size(); i++){
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)data[i] << " ";
    }
    std::cout << std::endl;

    connection->connect(global_status::server_ip(), 3308);
    ios.run();
}
