#include "../../include/one/taskrunner.hpp"

//
// Initializes and adds a new task to the runner
//
void TaskRunner::addTask(std::unique_ptr<Task> task) {
    if (task->init(this)) {
        this->tasks.push_back(std::move(task));
    }
}

//
// Run all tasks - remove the completed ones
//
void TaskRunner::run() {
    while (this->running) {

        // How to exit the outer loop? ^
        // Add reference to parent --> set running variable from task?


        // Process every task once per iteration
        for (auto iterator = this->tasks.begin(); iterator != this->tasks.end(); iterator++) {
            // Task is done - remove it from the list
            if (!(*iterator)->run()) {
                (*iterator)->shutdown();
                this->tasks.erase(iterator);
            }
        }
    }

    // Task runner is exiting - allow all tasks to do their cleanup
    for (auto& task : this->tasks) {
        task->shutdown();
    }
}