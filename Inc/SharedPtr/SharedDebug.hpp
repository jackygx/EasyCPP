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

#ifndef __SHARED_DEBUG_HPP__
#define __SHARED_DEBUG_HPP__

#include <stdio.h>
#include <ExceptionSimple.hpp>
#include <Meta/Meta.hpp>
#include <Debug/TypeDebug.hpp>

#define DEBUG_SPTR

#ifdef DEBUG_SPTR

extern bool DebugSharedPtr;

#define SPTR_DEBUG(fmt, ...) \
	if (DebugSharedPtr) { \
		printf("[%4d] " fmt "\n", __LINE__, ##__VA_ARGS__); \
	}

#define SPTR_PRINT(fmt, ...) \
	if (DebugSharedPtr) { \
		printf(fmt, ##__VA_ARGS__); \
	}

#define SPTR_VARIADIC_PRINT(fmt, data) \
	if (DebugSharedPtr) { \
		EXPAND(printf(fmt, data)); \
	}

#define START_DEBUG_SHARED_PTR() DebugSharedPtr = true
#define STOP_DEBUG_SHARED_PTR() DebugSharedPtr = false

class CSharedPtrEnableDebug
{
public:
	inline CSharedPtrEnableDebug(void)
	{
		DebugSharedPtr = true;
	}

	inline ~CSharedPtrEnableDebug(void)
	{
		DebugSharedPtr = false;
	}
};

#else /* !DEBUG_SPTR */
#define SPTR_DEBUG(fmt, ...)
#define SPTR_PRINT(fmt, ...)
#define SPTR_VARIADIC_PRINT(fmt, data)
#endif /* DEBUG_SPTR */

#define DEBUG_TEMPLATE \
	class This = T
#define DECLARE_DEBUG_TEMPLATE \
	class This

#endif /* __SHARED_DEBUG_HPP__ */

