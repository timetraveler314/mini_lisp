//
// Created by timetraveler314 on 5/18/24.
//

#ifndef MINI_LISP_TASK_H
#define MINI_LISP_TASK_H

#include <coroutine>
#include <optional>
#include <vector>

namespace Utils {
    template<typename T>
    struct Task {
        struct promise_type;
        using handle_t = std::coroutine_handle<promise_type>;

        struct promise_type {
            Task* t;

            Task<T> get_return_object() {
                return { handle_t::from_promise(*this) };
            }

            std::suspend_never initial_suspend() noexcept {
                return {};
            }

            std::suspend_never final_suspend() noexcept {
                for (auto& handle : t->waiting) handle.resume();
                return {};
            }

            void return_value(const T& v) {
                t->ret = v;
            }

            void return_value(T&& v) {
                t->ret = v;
            }

            void unhandled_exception() {}
        };

        handle_t handle;
        std::optional<T> ret;
        std::vector<std::coroutine_handle<>> waiting;

        Task(handle_t handle) : handle(handle) {
            handle.promise().t = this;
        }

        bool ready() {
            return ret.has_value();
        }

        std::optional<T>& get_result() {
            return ret;
        }

        auto operator co_await() {
            struct Awaiter {
                Task& t;

                Awaiter(Task& t) : t(t) {}

                bool await_ready() {
                    return t.ret.has_value();
                }

                void await_suspend(std::coroutine_handle<> handle) {
                    t.waiting.emplace_back(handle);
                }

                T await_resume() {
                    return t.ret.value();
                }
            };
            return Awaiter{ *this };
        }
    };
}

#endif //MINI_LISP_TASK_H
