#ifndef _TINYRUSTC_HASH_HXX_2022_11_07_13_16_31
#define _TINYRUSTC_HASH_HXX_2022_11_07_13_16_31

#include <type_traits>
#include <bit>
#include <cstddef>
#include <array>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <tuple>
#include <utility>
#include <variant>
#include <span>

#include <utility.hxx>

namespace trustc{

    template<typename T> constexpr T _hash_prime(){
        if constexpr(sizeof(T)==4)
            return 16777619ul;
        else if constexpr(sizeof(T)==8)
            return 1099511628211ul;
        else
            static_assert(sizeof(T)==8,"Invalid bittedness");
    }

    template<typename T> constexpr T _hash_offset(){
        if constexpr(sizeof(T)==4)
            return 2166136261ul;
        else if constexpr(sizeof(T)==8)
            return 14695981039346656037ul;
        else
            static_assert(sizeof(T)==8,"Invalid bittedness");
    }


    constexpr inline std::size_t hash_prime = _hash_prime<std::size_t>();
    constexpr inline std::size_t hash_offset = _hash_offset<std::size_t>();

    template<typename T, typename is_scalar> struct _default_hasher{
        _default_hasher()=delete;
        _default_hasher(const _default_hasher&)=delete;
        _default_hasher(_default_hasher&&)=delete;
        _default_hasher& operator=(const _default_hasher&)=delete;
        _default_hasher& operator=(_default_hasher&&)=delete;
        ~_default_hasher()=delete;
        void operator()(T)=delete;
    };

    template<typename T> struct _default_hasher<T,std::true_type>{

        constexpr std::size_t operator()(T val) const noexcept{
            auto buf = std::bit_cast<std::array<unsigned char,sizeof(T)>>(val);
            std::size_t hash_val = hash_offset;
            for(auto&& a : buf){
                hash_val *= hash_prime;
                hash_val ^= static_cast<std::size_t>(a);
            }
            return hash_val;
        }
    };

    template<typename T=void> struct hasher : _default_hasher<T, typename std::is_scalar<T>::type>{};


    template<> struct hasher<void>{
        
        template<typename T> auto operator()(const T& t) const noexcept(noexcept(hasher<T>{}(t))) -> decltype(hasher<T>{}(t)){
            return hasher<T>{}(t);
        }
    };


    template<typename T, std::size_t N> struct hasher<T[N]>{
    private:
        hasher<T> inner;
    public:
        constexpr std::size_t operator()(const T(&arr)[N]) const noexcept(noexcept(hasher<T>{}(arr[0]))) requires requires(const T& val){this->inner(val);}{
            std::size_t hash_val = hash_offset;
            for(auto&& v : arr){
                hash_val *= hash_prime;
                hash_val ^= inner(v);
            }
            return hash_val;
        }
    };

    template<typename T, std::size_t N> struct hasher<std::array<T,N>>{
    private:
        hasher<T> inner;
    public:
        constexpr std::size_t operator()(const T(&arr)[N]) const noexcept(noexcept(hasher<T>{}(arr[0]))) requires requires(const T& val){this->inner(val);}{
            std::size_t hash_val = hash_offset;
            for(auto&& v : arr){
                hash_val *= hash_prime;
                hash_offset ^= inner(v);
            }
            return hash_val;
        }
    };

    template<typename... Ts> struct hasher<std::tuple<Ts...>>{
    private:
        std::tuple<hasher<Ts>...> inner;
    public:
        constexpr std::size_t operator()(const std::tuple<Ts...>& tup) const noexcept((noexcept(hasher<Ts>{}(std::declval<Ts>())) && ... && true)){
            std::size_t hash_val = hash_offset;

            trustc::static_repeat([&hash_val](auto&& val, auto&& hasher){
                hash_val *= hash_prime;
                hash_val ^= hasher(val);
            },tup,inner);

            return hash_val;
        }
    };

    template<typename T,typename U> struct hasher<std::pair<T,U>>{
    private:
        std::pair<hasher<T>,hasher<U>> inner;
    public:
        constexpr std::size_t operator()(const std::pair<T,U>& pair) const noexcept((noexcept(hasher<T>{}(pair.first))&&noexcept(hasher<U>{}(pair.second)))){
            std::size_t hash_val = hash_offset;

            hash_val *= hash_prime;
            hash_val ^= inner.first(pair.first);
            hash_val *= hash_prime;
            hash_val ^= inner.second(pair.second);

            return hash_val;
        }
    };

    template<typename T,typename Allocator> struct hasher<std::vector<T,Allocator>>{
    private:
        hasher<T> inner;
    public:
        constexpr std::size_t operator()(const std::vector<T>& vec) const noexcept(noexcept(hasher<T>{}(vec[0]))){
            std::size_t hash_val = hash_offset;
            for(auto&& v : vec){
                hash_val *= hash_prime;
                hash_offset ^= inner(v);
            }
            return hash_val;
        }
    };

    template<typename CharT,typename CharTraits> struct hasher<std::basic_string_view<CharT,CharTraits>>{
    private:
        hasher<CharT> inner;
    public:
        constexpr std::size_t operator()(const std::basic_string_view<CharT,CharTraits>& vec) const noexcept(noexcept(hasher<CharT>{}(vec[0]))){
            std::size_t hash_val = hash_offset;
            for(auto&& v : vec){
                hash_val *= hash_prime;
                hash_offset ^= inner(v);
            }
            return hash_val;
        }
    };
    template<typename CharT,typename CharTraits,typename Allocator> struct hasher<std::basic_string<CharT,CharTraits,Allocator>>{
    private:
        hasher<CharT> inner;
    public:
        constexpr std::size_t operator()(const std::basic_string<CharT,CharTraits,Allocator>& vec) const noexcept(noexcept(hasher<CharT>{}(vec[0]))){
            std::size_t hash_val = hash_offset;
            for(auto&& v : vec){
                hash_val *= hash_prime;
                hash_offset ^= inner(v);
            }
            return hash_val;
        }
    };

    template<typename T,std::ptrdiff_t Extent> struct hasher<std::span<T,Extent>>{
    private:
        hasher<T> inner;
    public:
        constexpr std::size_t operator()(const std::span<T,Extent>& vec) const noexcept(noexcept(hasher<T>{}(vec[0]))){
            std::size_t hash_val = hash_offset;
            for(auto&& v : vec){
                hash_val *= hash_prime;
                hash_offset ^= inner(v);
            }
            return hash_val;
        }
    };

    template<typename T,typename Compare,typename Allocator> struct hasher<std::set<T,Compare,Allocator>>{
    private:
        hasher<T> inner;
    public:
        constexpr std::size_t operator()(const std::set<T,Compare,Allocator>& set) const noexcept(noexcept(hasher<T>{}(*set.begin()))){
            std::size_t hash_val = hash_offset;

            for(auto&& v : set){
                hash_val ^= inner(v);
            }
            return hash_val;
        }
    };

    template<typename T,typename Hash,typename Eq,typename Allocator> struct hasher<std::unordered_set<T,Hash,Eq,Allocator>>{
    private:
        hasher<T> inner;
    public:
        constexpr std::size_t operator()(const std::unordered_set<T,Hash,Eq,Allocator>& set) const noexcept(noexcept(hasher<T>{}(*set.begin()))){
            std::size_t hash_val = hash_offset;

            for(auto&& v : set){
                hash_val ^= inner(v);
            }

            return hash_val;
        }
    };


}

#endif /* _TINYRUSTC_HASH_HXX_2022_11_07_13_16_31 */