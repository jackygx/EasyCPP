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

#ifndef __MACRO_HPP__
#define __MACRO_HPP__

#define TO_STRING(T)    TO_STRING1(T)
#define TO_STRING1(T)   #T

#define CONNECT(a, b)	CONNECT1(a, b)
#define CONNECT1(a, b)	a##b

#define ARRAY_AND_SIZE(arr) (arr), (sizeof(arr) - 1)
#define ARRAY_SIZE(arr) (uint32_t)(sizeof(arr) / sizeof(arr[0]))

#define NARG(...)		_NARG(__VA_ARGS__, __NARG())
#define _NARG(...)		___NARG(__VA_ARGS__)
#define __NARG()		63, 62, 61, 60, 59, 58, 57, 56, \
						55, 54, 53, 52, 51, 50, 49, 48, \
						47, 46, 45, 44, 43, 42, 41, 40, \
						39, 38, 37, 36, 35, 34, 33, 32, \
						31, 30, 29, 28, 27, 26, 25, 24, \
						23, 22, 21, 20, 19, 18, 17, 16, \
						15, 14, 13, 12, 11, 10,  9,  8, \
						 7,  6,  5,  4,  3,  2,  1,  0
#define ___NARG(		 _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8, \
						 _9, _10, _11, _12, _13, _14, _15, _16, \
						_17, _18, _19, _20, _21, _22, _23, _24, \
						_25, _26, _27, _28, _29, _30, _31, _32, \
						_33, _34, _35, _36, _37, _38, _39, _40, \
						_41, _42, _43, _44, _45, _46, _47, _48, \
						_49, _50, _51, _52, _53, _54, _55, _56, \
						_57, _58, _59, _60, _61, _62, _63, N, ...) N

#endif /* __MACRO_HPP__ */

