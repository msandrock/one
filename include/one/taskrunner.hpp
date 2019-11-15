#ifndef RUNNER_HPP
#define RUNNER_HPP
#include "task.hpp"
#include <memory>
#include <vector>

class TaskRunner {
public:
    void addTask(std::unique_ptr<Task> task);
    void run();
    // Allow tasks to terminate the runner and add other tasks
    friend Task;

private:
    std::vector<std::unique_ptr<Task>> tasks;
    bool running = true;
};

#endif