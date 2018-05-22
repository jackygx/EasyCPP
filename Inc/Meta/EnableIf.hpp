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

#ifndef __ENABLE_IF_HPP__
#define __ENABLE_IF_HPP__

#define DEFINE_ENABLE_IF(__type) \
	template <bool, typename _Tp = __type> \
	struct enable_if_##__type {}; \
	template <typename _Tp> \
	struct enable_if_##__type<true, _Tp> { typedef _Tp type; }

constexpr bool AllTrue(void)
{
	return true;
}

template<class... Args>
constexpr bool AllTrue(bool arg, Args... args)
{
	return arg && AllTrue(static_cast<bool>(args)...);
}

DEFINE_ENABLE_IF(void);
DEFINE_ENABLE_IF(int);
DEFINE_ENABLE_IF(bool);

#define ENABLE_IF(...) \
	typename enable_if_bool<__VA_ARGS__::value>::type = false

#define DECLARE_ENABLE_IF(...) \
	typename enable_if_bool<__VA_ARGS__::value>::type

#define ENABLE_IFS(...) \
	typename enable_if_bool<AllTrue(__VA_ARGS__::value...)>::type = false

#define DECLARE_ENABLE_IFS(...) \
	typename enable_if_bool<AllTrue(__VA_ARGS__::value...)>::type

#define ENABLE_IFEQ(a, b) \
	typename enable_if_bool<a == b>::type = false

#define DECLARE_ENABLE_IFEQ(a, b) \
	typename enable_if_bool<a == b>::type

#define ENABLE_IFNEQ(a, b) \
	typename enable_if_bool<a != b>::type = false

#define DECLARE_ENABLE_IFNEQ(a, b) \
	typename enable_if_bool<a != b>::type

#endif /* __ENABLE_IF_HPP__ */

