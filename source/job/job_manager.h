/***********************************************************************************************************************
 * @author Hackman Lo
 * @file job_manager.h
 * @description 
 * @created_at 2023-10-24
***********************************************************************************************************************/

#ifndef AGENT_CLIENT_JOB_MANAGER_H
#define AGENT_CLIENT_JOB_MANAGER_H
#include <mutex>
#include <memory>
#include "base_job.h"
#include "agent/packages/protos/rendering_job.pb.h"
class job_manager {
public:
    static void add_job(std::shared_ptr<base_job> job_item);
    job_manager();
    struct job_data{
    public:
        job_data(std::shared_ptr<base_job> job) : job_(job){};
        std::mutex mutex_;
        std::shared_ptr<base_job> job_;
    };
protected:

    std::mutex jobs_mutex_;
    std::vector<std::shared_ptr<job_data>> jobs_;

    static std::shared_ptr<job_manager>& check_ptr_();
    static std::shared_ptr<job_manager> self_ptr_;

    std::mutex notify_mutex_;
    std::unique_lock<std::mutex> notify_lock_;
    std::condition_variable notify_;

    std::thread worker_;
    void job_worker();
};


#endif //AGENT_CLIENT_JOB_MANAGER_H
