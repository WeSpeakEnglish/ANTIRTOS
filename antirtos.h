// under MIT license, Aleksei Tertychnyi
#ifndef _antirtos_h
#define _antirtos_h
#include <cstddef>
#include <tuple>
#include <utility>

namespace antirtos
{

namespace utils
{
    template <std::size_t... IndexList>
    struct index_sequence
    {
        template <std::size_t n>
        using Append = index_sequence<IndexList..., n>;
    };

    template <std::size_t SequenceSize>
    struct make_index_sequence
    {
        using Type = typename make_index_sequence<SequenceSize - 1>::Type::template Append<SequenceSize - 1>;
    };

    template <>
    struct make_index_sequence<0u>
    {
        using Type = index_sequence<>;
    };

    template <typename... TypeList>
    using IndexSequenceFor = typename make_index_sequence<sizeof...(TypeList)>::Type;

} // namespace utils

template <typename... TArgs>
class Task
{
public:
    using FunctionPointer = void (*)(TArgs...);
    using FunctionArgs = std::tuple<TArgs...>;

    Task() : func_ptr(nullptr) {}
    ~Task() = default;

    Task(const Task &other) : func_ptr(other.func_ptr), func_args(other.func_args) {}
    inline Task &operator=(const Task &other)
    {
        if (this != &other)
        {
            func_ptr = other.func_ptr;
            func_args = other.func_args;
        }
        return *this;
    }

    inline Task(Task &&other) noexcept : func_ptr(other.func_ptr), func_args(std::move(other.func_args))
    {
        other.func_ptr = nullptr;
    }

    inline Task &operator=(Task &&other) noexcept
    {
        if (this != &other)
        {
            func_ptr = other.func_ptr;
            func_args = std::move(other.func_args);
            other.func_ptr = nullptr;
        }
        return *this;
    }

    inline void set(FunctionPointer f_p, TArgs... margs)
    {
        func_ptr = f_p;
        func_args = std::make_tuple(std::forward<TArgs>(margs)...);
    }

    inline void reset() { func_ptr = nullptr; }

    inline bool runable() const { return (func_ptr != nullptr); }

    inline int run()
    {
        if (func_ptr)
        {
            call_func(utils::IndexSequenceFor<TArgs...>());
            return 0;
        }
        return -1;
    }

    FunctionPointer function_pointer() const { return func_ptr; }

private:
    FunctionPointer func_ptr;
    FunctionArgs func_args;

    template <std::size_t... indexes>
    void call_func(utils::index_sequence<indexes...>)
    {
        func_ptr(std::get<indexes>(func_args)...);
    }
};

template <std::size_t QSize, typename... TArgs>
class TaskQ
{
public:
    TaskQ() : front(0), back(0), count(0) {}
    ~TaskQ() = default;

    int push(typename Task<TArgs...>::FunctionPointer f_p, TArgs... margs)
    {
        if (count < QSize)
        {
            tasks[back].set(f_p, std::forward<TArgs>(margs)...);
            back = (back + 1) % QSize;
            ++count;
            return 0;
        }
        return 1; // Queue is full
    }

    int push(Task<TArgs...> task)
    {
        if (count < QSize)
        {
            tasks[back] = task;
            back = (back + 1) % QSize;
            ++count;
            return 0;
        }
        return 1; // Queue is full
    }

    int pull()
    {
        if (count > 0)
        {
            int status = tasks[front].run();
            tasks[front].reset();
            front = (front + 1) % QSize;
            --count;
            return status;
        }
        return -1; // Queue is empty
    }

    void pull_all()
    {
        while (pull() == 0) {}
    }

private:
    std::size_t front;
    std::size_t back;
    std::size_t count;
    Task<TArgs...> tasks[QSize];
};




/// @brief Delayed task queue implementation supporting task scheduling.
template <std::size_t QSize, typename... TArgs>
class TaskQd : public TaskQ<QSize, TArgs...>
{
public:
    TaskQd() : time(0), count(0) {}
    ~TaskQd() = default;

    int push_delayed(typename Task<TArgs...>::FunctionPointer f_p, TArgs... margs, std::size_t delay_time)
    {
        if (count < QSize)
        {
            std::size_t exec_time = time + delay_time;

            std::size_t i;
            for (i = count; i > 0 && exec_times[i - 1] > exec_time; --i)
            {
                delayed_tasks[i] = std::move(delayed_tasks[i - 1]);
                exec_times[i] = exec_times[i - 1];
            }

            delayed_tasks[i].set(f_p, std::forward<TArgs>(margs)...);
            exec_times[i] = exec_time;
            ++count;
            return 0;
        }
        return 1; // Queue is full
    }

    bool revoke(typename Task<TArgs...>::FunctionPointer f_p)
    {
        bool found = false;
        for (std::size_t i = 0; i < count;)
        {
            if (delayed_tasks[i].runable() && delayed_tasks[i].function_pointer() == f_p)
            {
                for (std::size_t j = i; j < count - 1; ++j)
                {
                    delayed_tasks[j] = std::move(delayed_tasks[j + 1]);
                    exec_times[j] = exec_times[j + 1];
                }
                --count;
                found = true;
            }
            else
            {
                ++i;
            }
        }
        return found;
    }

    void tick()
    {
        while (count > 0 && exec_times[0] == time)
        {
            this->push(delayed_tasks[0]);
            for (std::size_t i = 0; i < count - 1; ++i)
            {
                delayed_tasks[i] = std::move(delayed_tasks[i + 1]);
                exec_times[i] = exec_times[i + 1];
            }
            --count;
        }
        ++time;
    }

private:
    std::size_t time;
    std::size_t count;
    Task<TArgs...> delayed_tasks[QSize];
    std::size_t exec_times[QSize];
};

} // namespace antirtos

#endif
