#ifndef _TINYRUSTC_UTILITY_HXX_2022_1_07_13_48_22
#define _TINYRUSTC_UTILITY_HXX_2022_1_07_13_48_22

#include <utility>

#include <tuple>
#include <type_traits>
#include <functional>
#include <vector>
#include <cstdint>

#include <compare>
#include <exception>

#include <iostream>
#include <string_view>

#ifndef __has_builtin
#define trustc_has_builtin(x) (0)
#else
#define trustc_has_builtin(x) __has_builtin(x)
#endif

#if trustc_has_builtin(__builtin_unreachable)
#define unreachable_unchecked() __builtin_unreachable()
#else
#define unreachable_unchecked() ((void)(*(int*)(0)))
#endif

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

    [[noreturn]] inline constexpr void unreachable_unchecked(){
#if trustc_has_builtin(__builtin_unreachable)
        __builtin_unreachable();
#endif
    }


    inline constexpr void assume(bool x){
#if trustc_has_builtin(__builtin_assume)
        __builtin_assume(x);
#else
        if(!x)
            unreachable_unchecked();
#endif
    }


    inline constexpr void debug_assert(bool val){
#if NDEBUG
        assume(val);
#else
        if(!val)
            std::terminate();
#endif
    }


    struct uint128{
    private:
        uint64_t lo;
        uint64_t hi;
    public:
        uint128() noexcept = default;

        constexpr uint128(uint64_t lo, uint64_t hi = uint64_t{}) noexcept : lo{lo}, hi{hi}{}

        constexpr uint128& operator+=(const uint128& other) noexcept {
            this->lo += other.lo;

            if(this->lo<other.lo)
                this->hi++;

            this->hi += other.hi;

            return *this;
        }
        constexpr uint128& operator-=(const uint128& other) noexcept {
            auto orig_lo = this->lo;

            this->lo -= other.lo;

            if(this->lo>orig_lo)
                this->hi--;
            this->hi -= other.hi;

            return *this;
        }

        constexpr auto operator<=>(const uint128&) const noexcept = default;
        constexpr bool operator==(const uint128&) const noexcept = default;

        constexpr uint128& operator |=(const uint128& other) noexcept{
            this->lo |= other.lo;
            this->hi |= other.hi;
            return *this;
        }
        constexpr uint128& operator &=(const uint128& other) noexcept{
            this->lo &= other.lo;
            this->hi &= other.hi;
            return *this;
        }

        constexpr uint128& operator ^=(const uint128& other) noexcept{
            this->lo ^= other.lo;
            this->hi ^= other.hi;
            return *this;
        }

        constexpr uint128& operator<<=(unsigned val) noexcept{
            debug_assert(val<128);
            if(val==0)
                return *this;
            if(val<64){
                auto carry = this->lo>>(64-val);
                this->lo<<=val;
                this->hi<<=val;
                this->hi|=carry;
            }else{
                val-=64;
                this->hi = this->lo;
                this->lo = 0;
                this->hi <<=val;
            }

            return *this;
        }

        constexpr uint128& operator>>=(unsigned val) noexcept{
            debug_assert(val<128);
            if(val==0)
                return *this;
            if(val<64){
                auto carry = this->hi<<(64-val);
                this->hi>>=val;
                this->lo>>=val;
                this->lo|=carry;
            }else{
                val-=64;
                this->lo = this->hi;
                this->hi = 0;
                this->lo <<=val;
            }

            return *this;
        }


        constexpr friend uint128 operator+(uint128 a,uint128 b) noexcept{
            a+=b;
            return a;
        }

        constexpr friend uint128 operator-(uint128 a, uint128 b) noexcept{
            return a-=b;
        }

        constexpr friend uint128 operator&(uint128 a, uint128 b) noexcept{
            a&=b;
            return a;
        }

        constexpr friend uint128 operator|(uint128 a, uint128 b) noexcept{
            a|=b;
            return a;
        }

        constexpr friend uint128 operator<<(uint128 a, unsigned b)noexcept{
            a<<=b;
            return a;
        }
        constexpr friend uint128 operator>>(uint128 a, unsigned b)noexcept{
            a>>=b;
            return a;
        }

    };

    using namespace std::literals;
    constexpr uint128 parse_uint128(std::string_view sv, int radix=0){
        constexpr const char alpha[]{"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
        if(radix!=0&&(radix<2||radix>36))
            throw std::out_of_range{"Invalid radix, range must be within [2,36]"};

        if(radix==0){
            if(sv.starts_with("0x")){
                radix=16;
                sv=sv.substr(2);
            }else if(sv.starts_with("0b")){
                radix=2;
                sv=sv.substr(2);
            }else if(sv.starts_with("0")){
                radix=8;
                sv=sv.substr(1);
            }else{
                radix=10;
            }
        }

        std::uint64_t lo{};
        std::uint64_t hi{};
        std::uint64_t hi2{};

        for(auto&& c : sv){
            auto dig = std::find(alpha,alpha+radix,c)-alpha;
            if(dig==radix)
                throw std::invalid_argument{"Invalid char "s+c+" in string, expected a digit"s};

            lo *= radix;
            hi *= radix;
            hi2 *= radix;
            lo += dig;
            hi += (lo>>48);
            hi2 += (hi>>48);

            if((hi2>>32))
                throw std::out_of_range{"String "s+std::string{sv}+" is not a valid uint128"};

            lo&=0xFFFFFFFFFFFF;
            hi&=0xFFFFFFFFFFFF;
        }

        lo |= (hi&0xFFFF)<<48;
        hi>>=16;
        hi |= hi2;

        return uint128{lo,hi};
    }


    constexpr uint128 operator""_u128(const char* p){
        return parse_uint128(p,0);
    }

    template<typename CharT,typename CharTraits,typename... Fs> struct string_switch;

    template<typename ViewTy, typename T> struct match_pattern{};

    template<typename CharT,typename CharTraits> struct match_pattern<std::basic_string_view<CharT,CharTraits>,std::basic_string_view<CharT,CharTraits>>{
    public:
        constexpr static bool matches(std::basic_string_view<CharT,CharTraits> key, std::basic_string_view<CharT,CharTraits> pattern) noexcept{
            return key==pattern;
        }
    };

    template<typename CharT,typename CharTraits> struct multipattern{
    private:
        std::initializer_list<std::basic_string_view<CharT,CharTraits>> pats;
    public:
        constexpr multipattern(std::initializer_list<std::basic_string_view<CharT,CharTraits>> pats) : pats{pats}{}
        constexpr bool matches(std::basic_string_view<CharT,CharTraits> key) const noexcept{
            for(auto&& a : this->pats){
                if(a==key)
                    return true;
            }

            return false;
        }
    };

    template<typename CharT,typename CharTraits> struct match_pattern<std::basic_string_view<CharT,CharTraits>,multipattern<CharT,CharTraits>>{
    public:
        constexpr static bool matches(std::basic_string_view<CharT,CharTraits> key, const multipattern<CharT,CharTraits>& pattern) noexcept{
            return pattern.matches(key);
        }
    };

    namespace _detail{
        template<typename ViewTy, typename Def> decltype(auto) match_view_impl(ViewTy val, Def&& def, std::index_sequence<>){
            return std::invoke(std::forward<Def>(def),val);
        }
        template<typename ViewTy, typename Def, typename Ar1, typename... Ars, std::size_t I1, std::size_t... Is> decltype(auto) match_view_impl(ViewTy val, Def&& def, std::index_sequence<I1,Is...>,Ar1&& match_one, Ars&& ...match_rest){
            using pattern = decltype(match_one.first);
            if(match_pattern<ViewTy,pattern>::matches(val,match_one.first))
                return std::invoke(std::forward<Ar1>(match_one).second);
            return match_view_impl(val,def,std::index_sequence<Is...>{}, std::move(match_rest)...);
        }
        template<typename ViewTy, typename Def, typename... Ar, std::size_t... Is> decltype(auto) match_view(ViewTy val, Def&& def, std::tuple<Ar...>&& fs, std::index_sequence<Is...>){
            return match_view_impl(val, std::forward<Def>(def), std::index_sequence<Is...>{}, std::get<Is>(std::move(fs))...);
        }

        template<typename Ar> struct extract_second{};
    }

    template<typename CharT,typename CharTraits,typename... Ar> struct string_switch{
    private:
        std::basic_string_view<CharT,CharTraits> to_match;
        std::tuple<Ar...> cases; 
    public:
        constexpr string_switch(std::basic_string_view<CharT,CharTraits> to_match, std::tuple<Ar...> cases) : to_match{to_match}, cases{std::move(cases)}{}
        constexpr string_switch(std::basic_string_view<CharT,CharTraits> sv) requires (sizeof...(Ar)==0) : to_match{sv}, cases{}{}

        constexpr string_switch(CharT* ptr)requires (sizeof...(Ar)==0) : to_match{ptr}, cases{}{}

        template<typename F> requires std::is_invocable_v<F> constexpr string_switch<CharT,CharTraits,Ar...,std::pair<std::basic_string_view<CharT,CharTraits>,F>> match(std::basic_string_view<CharT,CharTraits> match, F&& f) && noexcept{
            auto new_ctor = [match=match,f=std::forward<F>(f)](auto... a) {
                return std::make_tuple(std::move(a)..., std::make_pair(match,std::forward<F>(f)));
            };

            return {this->to_match, new_ctor(std::move(this->cases))};
        }

        template<typename... Views,typename F> requires (std::is_invocable_v<F>&&(std::is_convertible_v<Views,std::basic_string_view<CharT,CharTraits>> && ...)) constexpr string_switch<CharT,CharTraits,Ar...,std::pair<multipattern<CharT,CharTraits>,F>> mutlimatch(F&& f, Views... views) && noexcept {
            auto new_ctor = [match=(multipattern<CharT,CharTraits>{views...}),f=std::forward<F>(f)](auto... a)mutable{
                return std::make_tuple(std::move(a)..., std::make_pair(match,std::forward<F>(f)));
            };

            return {this->to_match, std::apply(new_ctor,std::move(this->cases))};
        }


        template<typename Def> requires std::is_invocable_v<Def,std::basic_string_view<CharT,CharTraits>> 
            std::common_type_t<std::invoke_result_t<Def,std::basic_string_view<CharT,CharTraits>>,std::invoke_result_t<std::tuple_element_t<1,Ar>>...>
            or_else(Def&& def) && noexcept ((std::is_nothrow_invocable_v<Def,std::basic_string_view<CharT,CharTraits>> && ... && std::is_nothrow_invocable_v<std::tuple_element_t<1,Ar>>)){
                return _detail::match_view(this->to_match, std::forward<Def>(def), std::move(this->cases),std::index_sequence_for<Ar...>{});
        }
    };

    template<typename CharT,typename CharTraits> string_switch(std::basic_string_view<CharT,CharTraits>) -> string_switch<CharT,CharTraits>;
    template<typename CharT> string_switch(CharT*) -> string_switch<CharT,std::char_traits<CharT>>;
    template<typename CharT,typename CharTraits,typename Allocator> string_switch(const std::basic_string<CharT,CharTraits,Allocator>&) -> string_switch<CharT,CharTraits>;


}

#endif /* _TINYRUSTC_UTILITY_HXX_2022_1_07_13_48_22 */