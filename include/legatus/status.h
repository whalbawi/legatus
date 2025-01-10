#pragma once

#include <cstdint>

#include <utility>
#include <variant>

namespace legatus {

enum class None : uint8_t {};

template <typename T = None, typename E = None>
class Status {
  public:
    template <typename U>
    static Status<T, E> make_ok(U&& val);

    template <typename U>
    static Status<T, E> make_err(U&& val);

    static Status<T, E> make_ok()
        requires std::is_same_v<T, None>;

    static Status<T, E> make_err()
        requires std::is_same_v<E, None>;

    bool is_ok() const;

    bool is_err() const;

    T ok();

    E err();

  private:
    Status() = delete;

    explicit Status(std::variant<T, E> state);

    std::variant<T, E> state_;
};

template <typename T, typename E>
template <typename U>
Status<T, E> Status<T, E>::make_ok(U&& val) {
    return Status(std::variant<T, E>(std::in_place_index<0>, std::forward<U>(val)));
}

template <typename T, typename E>
Status<T, E> Status<T, E>::make_ok()
    requires std::is_same_v<T, None>
{
    return make_ok(None{});
}

template <typename T, typename E>
template <typename U>
Status<T, E> Status<T, E>::make_err(U&& val) {
    return Status(std::variant<T, E>(std::in_place_index<1>, std::forward<U>(val)));
}

template <typename T, typename E>
Status<T, E> Status<T, E>::make_err()
    requires std::is_same_v<E, None>
{
    return make_err(None{});
}

template <typename T, typename E>
Status<T, E>::Status(std::variant<T, E> state) : state_(std::move(state)) {}

template <typename T, typename E>
bool Status<T, E>::is_ok() const {
    return state_.index() == 0;
}

template <typename T, typename E>
bool Status<T, E>::is_err() const {
    return state_.index() == 1;
}

template <typename T, typename E>
T Status<T, E>::ok() {
    return std::move(std::get<0>(state_));
}

template <typename T, typename E>
E Status<T, E>::err() {
    return std::move(std::get<1>(state_));
}

} // namespace legatus
