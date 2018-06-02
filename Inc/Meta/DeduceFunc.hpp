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

#ifndef __DEDUCE_FUNC_HPP__
#define __DEDUCE_FUNC_HPP__

#include <memory>

#include "Pack.hpp"
#include "EnableIf.hpp"
#include "HasMember.hpp"

template <class...>
using void_t = void;

template <class...>
using bool_t = bool;

template <class T, typename = void>
struct deduce_function
{
	/* Does nothing */
};

template<class Ret, class C, class... Tn>
struct deduce_function<Ret (C::*)(Tn...)>
{
	using ret_type = Ret;
	using class_type = C;
	static const int nParam = sizeof...(Tn);
	using args_type = pack<Tn...>;
	using const_type = std::false_type();
	using is_func = std::true_type();
};

template<class Ret, class C, class... Tn>
struct deduce_function<Ret (C::*)(Tn...) const>
{
	using ret_type = Ret;
	using class_type = C;
	static const int nParam = sizeof...(Tn);
	using args_type = pack<Tn...>;
	using const_type = std::true_type();
	using is_func = std::true_type();
};

template <class Ret, class... Tn>
struct deduce_function<Ret(Tn...)>
{
	using ret_type = Ret;
	using class_type = void;
	static const int nParam = sizeof...(Tn);
	using args_type = pack<Tn...>;
	using const_type = std::false_type();
	using is_func = std::true_type();
};

template <typename T>
struct deduce_function<T, void_t<decltype(&T::operator())>>
{
	using ret_type = typename deduce_function<decltype(
		&std::remove_reference<T>::type::operator())>::ret_type;
	using class_type = typename deduce_function<decltype(
		&std::remove_reference<T>::type::operator())>::class_type;
	static const int nParam = deduce_function<decltype(
		&std::remove_reference<T>::type::operator())>::nParam;
	using args_type = typename deduce_function<decltype(
		&std::remove_reference<T>::type::operator())>::args_type;
	using const_type = typename deduce_function<decltype(
		&std::remove_reference<T>::type::operator())>::const_type;
	using is_func = std::true_type();
};

#define IS_FUNC_RET_VOID(fn) \
	std::is_void<typename deduce_function<fn>::ret_type>
#define IS_CONST_FUNC(fn) \
	std::is_same<typename deduce_function<fn>::const_type, std::true_type>

#define IS_SAME_RET(fn, type) \
	std::is_same<typename deduce_function<fn>::ret_type, type>
#define IS_SAME_ARGS(fn, ...) \
	std::is_same<typename deduce_function<fn>::args_type, pack<__VA_ARGS__>>

#define CLASS_TYPE(fn) typename deduce_function<fn>::class_type
#define IS_LAMBDA(fn) std::is_class<typename deduce_function<fn>::class_type>

/* FN_ARGS(fn): typename pack<args...> */
#define FN_ARGS(fn) typename deduce_function<fn>::args_type
#define NPARAM(fn) deduce_function<fn>::nParam

#define FN_RET_TYPE(fn) typename deduce_function<fn>::ret_type

class CIsFuncPointer
{
public:
	template <class Fn,
			 ENABLE_IF(!IS_LAMBDA(Fn))>
	inline void IsFuncPointer(void);

	template <class Fn,
			 ENABLE_IF(IS_LAMBDA(Fn)),
			 ENABLE_IFEQ(sizeof(Fn), 1)>
	inline void IsFuncPointer(void);
};

HAS_MEMBER(IsFuncPointer);

#define IS_FUNC_POINTER(Fn) \
	has_template_member_IsFuncPointer<CIsFuncPointer, pack<Fn>>

#endif /* __DEDUCE_FUNC_HPP__ */

