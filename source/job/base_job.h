/***********************************************************************************************************************
 * @author Hackman Lo
 * @file base_job.h
 * @description 
 * @created_at 2023-10-24
***********************************************************************************************************************/

#ifndef AGENT_CLIENT_BASE_JOB_H
#define AGENT_CLIENT_BASE_JOB_H
#include <thread>
#include <memory>
#include <mutex>
class base_job : public std::enable_shared_from_this<base_job> {
public:
    virtual ~base_job() = default;
    virtual void run() = 0;
protected:
    std::shared_ptr<base_job> self_holder_ = nullptr;
    std::thread working_thread_;

    std::mutex notify_mutex_;
    std::unique_lock<std::mutex> notify_lock_;
    std::condition_variable notify_;
};


#endif //AGENT_CLIENT_BASE_JOB_H
