/***********************************************************************************************************************
 * @author Hackman Lo
 * @file job_manager.cpp
 * @description 
 * @created_at 2023-10-24
***********************************************************************************************************************/

#include "job_manager.h"

std::shared_ptr<job_manager> job_manager::self_ptr_ = nullptr;

std::shared_ptr<job_manager> &job_manager::check_ptr_() {
    if (!self_ptr_)
        self_ptr_ = std::make_shared<job_manager>();
    return self_ptr_;
}

void job_manager::add_job(std::shared_ptr<base_job> job_item) {
    std::scoped_lock<std::mutex> lock(check_ptr_()->jobs_mutex_);
    check_ptr_()->jobs_.emplace_back(std::make_shared<job_data>(job_item));
    check_ptr_()->notify_.notify_one();
}

job_manager::job_manager() : notify_lock_(notify_mutex_), worker_([&] { this->job_worker(); }) {

}

void job_manager::job_worker() {
    do {
        if (jobs_.empty()) {
            notify_.wait(notify_lock_, [&]() { return !jobs_.empty(); });
        }
        jobs_mutex_.lock();
        auto ptr = jobs_.front();
        jobs_.erase(jobs_.begin());
        jobs_mutex_.unlock();

        ptr->job_->run();



    } while (true);
}

