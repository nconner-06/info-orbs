#ifndef TASK_FACTORY_H
#define TASK_FACTORY_H

#include "TaskManager.h"
#include <memory>

// Implementation of make_unique for older C++ standards
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&...args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class TaskFactory {
public:
    static std::unique_ptr<Task> createHttpGetTask(const String &url, const String &filter, Task::ResponseCallback callback, Task::PreProcessCallback preProcess = nullptr) {
        return make_unique<Task>(
            url, filter, callback, [url, filter, callback, preProcess]() {
                TaskFactory::httpGetTask(url, filter, callback, preProcess);
            },
            preProcess);
    }

    static std::unique_ptr<Task> createMqttTask(const String &topic, const String &filter, Task::ResponseCallback callback) {
        return make_unique<Task>(
            topic, filter, callback, []() {
                // Placeholder for MQTT task execution logic
            },
            nullptr);
    }

    static std::unique_ptr<Task> createHttpGetFileTask(const String &url, const String &fileName, Task::ResponseCallback callback) {
        return make_unique<Task>(
            url, fileName, callback, [url, fileName, callback]() {
                TaskFactory::httpGetFileTask(url, fileName, callback);
            },
            nullptr);
    }

    // Declare the httpGetTask method
    static void httpGetTask(const String &url, const String &filter, Task::ResponseCallback callback, Task::PreProcessCallback preProcess);
    static void httpGetFileTask(const String &url, const String &fileName, Task::ResponseCallback callback);
};

#endif // TASK_FACTORY_H
