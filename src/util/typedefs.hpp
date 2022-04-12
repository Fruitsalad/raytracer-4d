#pragma once

#include <limits>
#include <type_traits>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>

#ifdef ENABLE_THREADS
#include "extern/threadpool.hpp"
using thread_pool::ThreadPool;
#endif

using uint = unsigned int;
using byte = unsigned char;
using String = std::string;
using String$ = const String;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using std::make_unique;

template<class ...C> using Unique = std::unique_ptr<C...>;
template<class ...C> using Shared = std::shared_ptr<C...>;
template<class ...C> using Weak = std::weak_ptr<C...>;
template<class ...C> using List = std::vector<C...>;
template<class ...C> using Map = std::unordered_map<C...>;
template<class ...C> using MultiMap = std::unordered_multimap<C...>;
template<class ...C> using Function = std::function<C...>;
template<class ...C> using Limits = std::numeric_limits<C...>;

template<class C> constexpr C maxOf = Limits<C>::max();
template<class C> constexpr C minOf = Limits<C>::lowest();
#define max_of(...) (max_of<TypeOf(__VA_ARGS__)>)
#define min_of(...) (min_of<TypeOf(__VA_ARGS__)>)
#define $(...) (std::to_string(__VA_ARGS__))
