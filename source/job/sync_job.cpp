/***********************************************************************************************************************
 * @author Hackman Lo
 * @file sync_job.cpp
 * @description 
 * @created_at 2023-10-24
***********************************************************************************************************************/

#include "sync_job.h"
#include "global/global_status.h"
#include <filesystem>
#include <boost/process.hpp>
#include "agent/packages/protos/system.pb.h"
#include <sahara/utils/uuid.h>
sync_job::sync_job(agent::package::renderer::rendering_job job) : job_data_(std::move(job)) {

}

void sync_job::run() {
    self_holder_ = shared_from_this();
    working_thread_ = std::thread([&]() {
        bool succeed = false;
        std::string reason, data;
        for (int i = 0; i < 1; i++) {
            if (!sync_files()) {
                reason = "File Sync Failed";
                break;
            }

            std::filesystem::path project_file_path("./temp_file/project/" + job_data_.project().project_id() + "/" + job_data_.project().blend_file().relative_path());
            if (!std::filesystem::exists(project_file_path)) {
                reason = "Can't Find Project File";
                break;
            }

            std::filesystem::path blender_executable_path = get_blender_executable_path();
            if (!std::filesystem::exists(blender_executable_path)) {
                reason = "Blender Executable Not Exists At: " + blender_executable_path.string();
                break;
            }

            std::filesystem::path render_output_path("./temp_file/project/" + job_data_.project().project_id() + "/output/");
            std::filesystem::create_directories(render_output_path);

            std::stringstream ss;
            ss << '"' << std::filesystem::absolute(blender_executable_path).string() << "\" ";
            ss << "-b \"" << std::filesystem::absolute(project_file_path).string() << "\" ";
            ss << "-o \"" << boost::algorithm::replace_all_copy(std::filesystem::absolute(render_output_path).string(), "\\", "/") << "\" ";
            ss << "-F PNG ";
            ss << "-f " << job_data_.frame() << " ";
            ss << "-E CYCLES -- --cycles-device CUDA+CPU ";
            std::cout << ss.str() << std::endl;
            boost::process::child render_process(ss.str());
            boost::system::error_code err;
            render_process.wait(err);
            if (err || render_process.exit_code() != 0) {
                reason = err ? err.what() : "Blender Called But Return Code Is Not 0, Returned With " + std::to_string(render_process.exit_code());
                break;
            }
            std::stringstream upload_command;
            std::string filename = get_output_file_name(job_data_.frame());
            std::string uuid = sahara::utils::uuid::generate();
            upload_command << "curl -F \"file=@" << render_output_path.append(filename).string() << "\" ";
            upload_command << "\"http://" << global_status::server_ip() << ":" << global_status::server_http_port();
            upload_command << "/project/upload_result?project_id=" << job_data_.project().project_id()<< "&uuid=" << uuid << "\" ";
            data = "/" + job_data_.project().project_id() + "/output/" + filename;

            boost::process::child upload_process(upload_command.str());
            upload_process.wait(err);
            if(err || upload_process.exit_code() != 0){
                reason = err ? err.what() : "Upload Render Result Failed";
                break;
            }

            succeed = true;
            reason = "";
        }
        agent::package::client::dispatchable_status status;
        status.set_dispatchable_id_(job_data_.job_id());
        status.set_additional_data(data);
        status.set_reason_(reason);
        status.set_status_(succeed ? agent::package::client::dispatchable_status::DST_SUCCESS : agent::package::client::dispatchable_status::DST_FAILED);
        connection->send_package(PACKAGE_DISPATCHABLE_UPDATED, status);


    });

}

bool sync_job::sync_files() {
    std::vector<std::shared_ptr<std::thread>> threads_;
    auto remains_ = job_data_.project().files_size();
    std::atomic running_count_ = 0;
    for (int i = 0; i < job_data_.project().files_size(); ++i) {
        if (threads_.size() >= 10) {
            notify_.wait(notify_lock_, [&]() { return running_count_ < 10; });
        }
        running_count_ += 1;
        auto &file = job_data_.project().files(i);
        threads_.emplace_back(std::make_shared<std::thread>([&]() {
            std::filesystem::path file_path("./temp_file/project/" + job_data_.project().project_id() + "/" + file.relative_path());
            auto parent_path = file_path.parent_path().string();
            std::filesystem::create_directories(file_path.parent_path());
            bool success = false;
            if (!std::filesystem::exists(file_path)) {
                int retry_count_ = 0;
                while (retry_count_ < 3) {
                    std::cout << "curl " + file.download_url() + " --output " + std::filesystem::absolute(file_path).string() << std::endl;
                    boost::process::child get_process("curl " + file.download_url() + " --output " + std::filesystem::absolute(file_path).string());
                    boost::system::error_code err;
                    get_process.wait(err);
                    if (err || get_process.exit_code() != 0) {
                        retry_count_ += 1;
                        std::filesystem::remove(file_path);
                    }
                    success = true;
                    break;
                }
            }else success = true;
            running_count_ -= 1;
            remains_ -= success ? 1 : 0;
            notify_.notify_one();
        }));
    }
    for (auto &thread: threads_) {
        thread->join();
    }
    return remains_ == 0;
}

std::filesystem::path sync_job::get_blender_executable_path() {
    std::string blender_version = job_data_.project().blender_version().empty() ? "3.6" : job_data_.project().blender_version();
    std::string executable = "./blender/" + blender_version + "/";
#ifdef _WIN32
    executable += "blender.exe";
#else
    executable += "blender";
#endif
    return std::filesystem::path(executable);
}

std::string sync_job::get_output_file_name(std::uint32_t frame) {
    std::stringstream ss;
    ss << std::setw(4) << std::setfill('0') << frame << ".png";
    return ss.str();
}

