#pragma once
#include "delegate.hpp"
#include "edge.hpp"
#include "meta.hpp"
#include <tuple>
#include <vector>

namespace mini {

namespace meta {
namespace detail {

template <typename>
struct Type_List_To_Tuple_Impl;

// @TODO: support more than just this tuple
template <typename... Ts>
struct Type_List_To_Tuple_Impl<Type_List<Ts...>> {
    using type = std::tuple<Ts...>;
};
template <typename T>
using Type_List_To_Tuple = typename Type_List_To_Tuple_Impl<T>::type;

template <typename>
struct Decorate_Type_List_With_Relaxed_Edge_Impl;
template <typename... Ts>
struct Decorate_Type_List_With_Relaxed_Edge_Impl<Type_List<Ts...>> {
    using type = Type_List<Relaxed_Edge<Ts>...>;
};
template <typename T>
using Decorate_Type_List_With_Relaxed_Edge = typename Decorate_Type_List_With_Relaxed_Edge_Impl<T>::type;

template <typename>
struct Decorate_Type_List_With_Edge_Impl;
template <typename... Ts>
struct Decorate_Type_List_With_Edge_Impl<Type_List<Ts...>> {
    using type = Type_List<Edge<Ts>...>;
};
template <typename T>
using Decorate_Type_List_With_Edge = typename Decorate_Type_List_With_Edge_Impl<T>::type;

template <typename T>
using Params = typename meta::Callable_Traits<T>::param_list;

template <typename T>
using Returns = typename meta::Callable_Traits<T>::return_list;

template <typename T>
using Return_Type = typename meta::Callable_Traits<T>::return_type;

template <typename T>
using Node_Input = Type_List_To_Tuple<Decorate_Type_List_With_Relaxed_Edge<Params<T>>>;

template <typename T>
using Node_Output = Type_List_To_Tuple<Decorate_Type_List_With_Edge<Returns<T>>>;

} // namespace detail
} // namespace meta

template <typename T>
struct Node {
public:
    using Inputs = meta::detail::Node_Input<T>;
    using Outputs =  meta::detail::Node_Output<T>;
    using Ref_Outputs =  meta::detail::Node_Output<T>&;
    using Const_Ref_Outputs =  const meta::detail::Node_Output<T>&;

    template <typename... Args>
    Node(Inputs o, Args&&... args) :
        callable{ std::forward<Args&&>(args)... }, inputs{ o }, outputs{
            init(
                callable,
                inputs,
                std::make_index_sequence<meta::detail::Params<T>::size>{},
                std::make_index_sequence<meta::detail::Returns<T>::size>{})
        } {
        attach_callback_to_edges(std::make_index_sequence<meta::detail::Params<T>::size>{});
    }

    void operator()() {
        apply(
            std::make_index_sequence<meta::detail::Params<T>::size>{},
            std::make_index_sequence<meta::detail::Returns<T>::size>{});
        out_of_sync = false;
    }

    Ref_Outputs edges() noexcept { return outputs; }
    Const_Ref_Outputs edges() const noexcept { return outputs; }

    operator bool() const noexcept { return out_of_sync; }
    [[nodiscard]] bool outdated() const noexcept { return out_of_sync; }

    void on_outdated(Delegate<void(Node&)> delegate) { outdated_listeners.emplace_back(std::move(delegate)); }

private:
    template <size_t... Is, size_t... Os>
    void apply(std::index_sequence<Is...>, std::index_sequence<Os...>) {
        auto immediate = callable(std::get<Is>(inputs).get()...);
        if constexpr (meta::is_tuple_like<meta::detail::Return_Type<T>>) {
            (..., static_cast<void>(std::get<Os>(outputs) = std::move(std::get<Os>(immediate))));
        } else {
            std::get<0>(outputs) = std::move(immediate);
        }
    }

    template <size_t... Is, size_t... Os>
    static Outputs
        init(T& callable, Inputs& inputs, std::index_sequence<Is...>, std::index_sequence<Os...>) {
        if constexpr (meta::is_tuple_like<meta::detail::Return_Type<T>>) {
            auto immediate = std::invoke(callable, std::get<Is>(inputs).get()...);
            return { std::get<Os>(immediate)... };
        } else {
            return { std::invoke(callable, std::get<Is>(inputs).get()...) };
        }
    }

    template <size_t N>
    void callback() {
        if(!out_of_sync) {
            out_of_sync = true;
            for (auto& on_outdated : outdated_listeners) {
                on_outdated(*this);
            }
        }
        // should we also mark the others as dirty here?
    }

    template <size_t... Is>
    void attach_callback_to_edges(std::index_sequence<Is...>) {
        (..., [&]() {
            auto& input = std::get<Is>(inputs);
            input.on_changed({ connect<&Node::callback<Is>>, *this });
        }());
    }

private:
    T callable;
    Inputs inputs;
    Outputs outputs;
    std::vector<Delegate<void(Node&)>> outdated_listeners;

    bool out_of_sync = false;
};

} // namespace mini
