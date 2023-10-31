/***********************************************************************************************************************
 * @author Hackman Lo
 * @file sync_job.h
 * @description 
 * @created_at 2023-10-24
***********************************************************************************************************************/

#ifndef AGENT_CLIENT_SYNC_JOB_H
#define AGENT_CLIENT_SYNC_JOB_H

#include "base_job.h"
#include "agent/packages/protos/rendering_job.pb.h"
#include <filesystem>
class sync_job : public base_job{
public:
    sync_job(agent::package::renderer::rendering_job);
    ~sync_job() = default;

    void run();

protected:
    bool sync_files();
    std::filesystem::path get_blender_executable_path();
    std::string get_output_file_name(std::uint32_t frame);
    agent::package::renderer::rendering_job job_data_;
};


#endif //AGENT_CLIENT_SYNC_JOB_H
