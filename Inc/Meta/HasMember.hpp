/*
 * Copyright (c) 2018 Guo Xiang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __HAS_MEMBER_HPP__
#define __HAS_MEMBER_HPP__

#include <memory>

#include "Pack.hpp"

/* This MACRO is for test purpose ONLY */
#define TEST_MEMBER(func) \
	template <typename _U, typename... _U1> \
	struct test_member_##func { \
		typedef decltype(std::declval<_U>().func(std::declval<_U1>()...)) type; \
	}

/* Example to test member:
 * Adds below in the failed cpp file.
 * It cannot be added into any functions.
 *
 * TEST_MEMBER(Log);
 * test_member_Log<const CStr &, const ILoggerPtr &>::type *__test__ = NULL;
 */

#define HAS_MEMBER(func) \
	template <typename __M, typename... __M1> \
	struct has_member_##func { \
	private: \
		template<typename U> \
		constexpr static auto check(int) \
		->decltype(std::declval<U>().func(std::declval<__M1>()...), \
				std::true_type()) { \
			return std::true_type(); \
		} \
 \
		template<typename U> \
		constexpr static std::false_type check(...) { \
			return std::false_type(); \
		} \
	public: \
		enum { \
			value = std::is_same<decltype(check<__M>(0)), std::true_type>::value \
		}; \
	}; \
 \
	template <typename __M, typename... __M1> \
	struct has_pointer_member_##func { \
	private: \
		template<typename U> \
		constexpr static auto check(int) \
		->decltype(std::declval<U>()->func(std::declval<__M1>()...), \
				std::true_type()) { \
			return std::true_type(); \
		} \
 \
		template<typename U> \
		constexpr static std::false_type check(...) { \
			return std::false_type(); \
		} \
	public: \
		enum { \
			value = std::is_same<decltype(check<__M>(0)), std::true_type>::value \
		}; \
	}; \
 \
	template <typename... __Mn> \
	struct has_template_member_##func {}; \
 \
	template <typename __M, typename... __MTypes, typename... __MArgs> \
	struct has_template_member_##func<__M, pack<__MTypes...>, __MArgs...> { \
	private: \
		template<typename U> \
		constexpr static auto check(int) \
		->decltype(std::declval<U>().template func<__MTypes...>(std::declval<__MArgs>()...), \
				std::true_type()) { \
			return std::true_type(); \
		} \
 \
		template<typename U> \
		constexpr static std::false_type check(...) { \
			return std::false_type(); \
		} \
	public: \
		enum { \
			value = std::is_same<decltype(check<__M>(0)), std::true_type>::value \
		}; \
	}; \
 \
	template <typename __M, typename... __MTypes> \
	struct has_template_member_##func<__M, pack<__MTypes...>> { \
	private: \
		template<typename U> \
		constexpr static auto check(int) \
		->decltype(std::declval<U>().template func<__MTypes...>(), std::true_type()) { \
			return std::true_type(); \
		} \
 \
		template<typename U> \
		constexpr static std::false_type check(...) { \
			return std::false_type(); \
		} \
	public: \
		enum { \
			value = std::is_same<decltype(check<__M>(0)), std::true_type>::value \
		}; \
	}; \
 \
	template <typename... __Mn> \
	struct has_pointer_template_member_##func {}; \
 \
	template <typename __M, typename... __MTypes, typename... __MArgs> \
	struct has_pointer_template_member_##func<__M, pack<__MTypes...>, __MArgs...> { \
	private: \
		template<typename U> \
		constexpr static auto check(int) \
		->decltype(std::declval<U>()->template func<__MTypes...>(std::declval<__MArgs>()...), \
				std::true_type()) { \
			return std::true_type(); \
		} \
 \
		template<typename U> \
		constexpr static std::false_type check(...) { \
			return std::false_type(); \
		} \
	public: \
		enum { \
			value = std::is_same<decltype(check<__M>(0)), std::true_type>::value \
		}; \
	}; \
 \
	template <typename __M, typename... __MTypes> \
	struct has_pointer_template_member_##func<__M, pack<__MTypes...>> { \
	private: \
		template<typename U> \
		constexpr static auto check(int) \
		->decltype(std::declval<U>()->template func<__MTypes...>(), std::true_type()) { \
			return std::true_type(); \
		} \
 \
		template<typename U> \
		constexpr static std::false_type check(...) { \
			return std::false_type(); \
		} \
	public: \
		enum { \
			value = std::is_same<decltype(check<__M>(0)), std::true_type>::value \
		}; \
	}

#define HAS_STATIC_MEMBER(func) \
	template <typename _T, typename... __M1> \
	struct has_static_member_##func { \
	private: \
		template<typename U> \
		constexpr static auto check(int) \
		->decltype(U::func(std::declval<__M1>()...), std::true_type()) { \
			return std::true_type(); \
		} \
 \
		template<typename U> \
		constexpr static std::false_type check(...) { \
			return std::false_type(); \
		} \
	public: \
		enum { \
			value = std::is_same<decltype(check<_T>(0)), std::true_type>::value \
		}; \
	}

#define HAS_MEMBER_OPS(_name, ops) \
	template <typename T, typename Args> \
	struct has_member_operator_##_name { \
	private: \
		template<typename _T> \
		constexpr static auto check(int) \
		->decltype(std::declval<_T>().operator ops (std::declval<Args>()), std::true_type()) { \
			return std::true_type(); \
		} \
 \
		template<typename _T> \
		constexpr static std::false_type check(...) { \
			return std::false_type(); \
		} \
	public: \
		enum { \
			value = std::is_same<decltype(check<T>(0)), std::true_type>::value \
		}; \
	}

HAS_MEMBER_OPS(equal, =);
HAS_MEMBER_OPS(is_equal, ==);
HAS_MEMBER_OPS(add_equal, +=);
HAS_MEMBER_OPS(add, +);
HAS_MEMBER_OPS(bigger, >);
HAS_MEMBER_OPS(smaller, <);

#define HAS_OPS(_name, ops) \
	template <typename T1, typename T2> \
	struct has_operator_##_name { \
	private: \
		template<typename _T1> \
		constexpr static auto check(int) \
		->decltype(std::declval<_T1>() ops std::declval<T2>(), std::true_type()) { \
			return std::true_type(); \
		} \
 \
		template<typename _T1> \
		constexpr static std::false_type check(...) { \
			return std::false_type(); \
		} \
	public: \
		enum { \
			value = std::is_same<decltype(check<T1>(0)), std::true_type>::value \
		}; \
	}

HAS_OPS(equal, =);
HAS_OPS(is_equal, ==);
HAS_OPS(add_equal, +=);
HAS_OPS(add, +);
HAS_OPS(bigger, >);
HAS_OPS(smaller, <);

template <typename TTar, typename... TArgs>
struct has_constructor {
private:
	template<typename _TTar>
	constexpr static auto check(int)
	->decltype(_TTar(std::declval<TArgs>()...), std::true_type()) {
		return std::true_type();
	}

	template<typename _TTar>
	constexpr static std::false_type check(...) {
		return std::false_type();
	}

public:
	enum {
		value = std::is_same<decltype(check<TTar>(0)), std::true_type>::value
	};
};

template <typename _T, typename... _T1>
struct has_operator_func {
private:
	template<typename U>
	constexpr static auto check(int)
	->decltype(std::declval<U>().operator()(std::declval<_T1>()...), std::true_type()) {
		return std::true_type();
	}

	template<typename U>
	constexpr static std::false_type check(...) {
		return std::false_type();
	}

public:
	enum {
		value = std::is_same<decltype(check<_T>(0)), std::true_type>::value
	};
};

#endif /* __HAS_MEMBER_HPP__ */

