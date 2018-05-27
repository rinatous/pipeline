
#pragma once

#include <functional>
#include <utility>
#include <stdexcept>

typedef std::function<void()> pipeline_complete;

template<typename UserData, typename... Args> struct pipeline_task;

template <typename UserData, typename Task, typename... Tasks>
struct pipeline_task<UserData, Task, Tasks...>: public pipeline_task<UserData, Tasks...>
{
    typedef pipeline_task<UserData, Tasks...> pipeline_task_child;
    typedef Task task_type;

    UserData & ctx;
    task_type task;

    pipeline_task(UserData & c, pipeline_complete& complete):
        pipeline_task_child(c, complete),
        ctx(c)
    {};

    inline void run() {
        task(std::ref(ctx), std::bind(&pipeline_task_child::run, this));
    };
};

template <typename UserData, typename Task>
struct pipeline_task<UserData, Task>
{
    typedef Task task_type;

    UserData & ctx;
    task_type task;
    pipeline_complete & complete;

    pipeline_task(UserData & c, pipeline_complete& cmplt):
            ctx(c),
            complete(cmplt)
    {};

    inline void run() {
        task(std::ref(ctx), complete);
    };
};


template <typename UserData, typename... Tasks>
struct pipeline
{
    typedef pipeline_task<UserData, Tasks...> pipeline_tasks;

    pipeline_complete complete;
    UserData ctx;
    pipeline_tasks tasks;

    pipeline(UserData && c, pipeline_complete && cmplt):
            complete(std::move(cmplt)),
            ctx(std::move(c)),
            tasks(ctx, complete)
    {};
    pipeline(pipeline_complete && cmplt):
            complete(std::move(cmplt)),
            tasks(ctx, complete)
    {};
    pipeline(UserData && c):
            ctx(std::move(c)),
            tasks(ctx, std::bind(&pipeline::on_complete_wrap, this))
    {};

    void on_complete_wrap() {
        if(complete)
            complete();

        throw std::runtime_error("Undefined complete function");
    }

    inline void run() {
        tasks.run();
    };
};