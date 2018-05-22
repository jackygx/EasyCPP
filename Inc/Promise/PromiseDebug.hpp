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

#ifndef __PROMISE_DEBUG_HPP__
#define __PROMISE_DEBUG_HPP__

#include <stdio.h>

//#define DEBUG_PROMISE

#ifdef DEBUG_PROMISE

#define PROMISE_DEBUG(fmt, ...) \
	printf("[%4d] " fmt "\n", __LINE__, ##__VA_ARGS__);

#define PROMISE_INFO(color, fmt, ...) \
	printf(color "[%4d] " fmt COLOR_NONE "\n", __LINE__, ##__VA_ARGS__);

#define PROMISE_PRINT(fmt, ...) \
	printf(fmt, ##__VA_ARGS__);

#else /* !DEBUG_PROMISE */

#define PROMISE_DEBUG(fmt, ...)
#define PROMISE_INFO(color, fmt, ...)
#define PROMISE_PRINT(fmt, ...)

#endif /* DEBUG_PROMISE */

#endif /* __PROMISE_DEBUG_HPP__ */

