#ifndef _TINYRUSTC_UTILITY_HXX_2022_1_07_13_48_22
#define _TINYRUSTC_UTILITY_HXX_2022_1_07_13_48_22

#include <utility>

#include <tuple>
#include <type_traits>
#include <functional>


namespace trustc{

    namespace _detail{
        struct _not_same_size{};
        template<typename Tup1,typename... Tuples> auto _get_tuple_sizes_impl(std::enable_if_t<((std::tuple_size<Tup1>::value == std::tuple_size<Tuples>::value) && ... && true)>*) -> std::tuple_size<Tup1>;

        template<typename... Tuples> auto _get_tuple_sizes_impl(...) -> _not_same_size;

        template<typename... Tuples> using _get_tuple_sizes = decltype(_get_tuple_sizes_impl<std::remove_cvref_t<Tuples>...>(nullptr));

        template<std::size_t I,typename F,typename... Tuples> constexpr void _tuple_apply_once(F& f, Tuples&&... tups){
            using std::get;
            std::invoke(f,get<I>(std::forward<Tuples>(tups))...);
        }

        template<typename F,typename... Tuples> void _static_repeat_impl(F& f, Tuples&&... tuples,std::index_sequence<>){}

        template<typename F,typename... Tuples, std::size_t I,std::size_t... Res> void _static_repeat_impl(F& f, Tuples&&... tups, std::index_sequence<I,Res...>){
            _tuple_apply_once<I>(f,std::forward<Tuples>(tups)...);
            _static_repeat_impl<F,Tuples...>(f,std::forward<Tuples>(tups)...,std::index_sequence<Res...>{});
        }
    }

    /// @brief Applies f N times equal to the number of elements of each tuple. f is called with only one element from each Tuple
    /// @tparam F The function to apply, which must be invokable with each element set from `Tuples`
    /// @tparam ...Tuples The tuples to pass, which must support get<N>
    template<typename F, typename... Tuples> constexpr void static_repeat(F&& f, Tuples&&... tuples) {
        _detail::_static_repeat_impl<F,Tuples...>(f,std::forward<Tuples>(tuples)...,std::make_index_sequence<_detail::_get_tuple_sizes<Tuples...>::value>{});
    }
}

#endif /* _TINYRUSTC_UTILITY_HXX_2022_1_07_13_48_22 */