#ifndef TASK_HPP
#define TASK_HPP

class TaskRunner;

// Abstract base class for all tasks
class Task {
    TaskRunner* parent;
public:
    virtual bool init(TaskRunner* parent) {
        this->parent = parent;
        return true;
    }
    virtual bool run() = 0;
    virtual void shutdown() {}
    virtual ~Task();
};

#endif