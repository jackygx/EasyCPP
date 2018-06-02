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
#include <Common/Common.hpp>

#define DEBUG_SPTR

#ifdef DEBUG_SPTR

extern bool DebugSharedPtr;

#define SPTR_DEBUG(fmt, ...) \
	if (DebugSharedPtr) { \
		printf("[%4d] " fmt "\n", __LINE__, ##__VA_ARGS__); \
	}

#define SPTR_ERROR(fmt, ...) \
	printf(COLOR_RED "[%4d] " COLOR_NONE fmt "\n", __LINE__, ##__VA_ARGS__); \

#define SPTR_DEBUG_ENTRY(fmt, ...) \
	if (DebugSharedPtr) { \
		printf("[%4d] ++ " fmt "\n", __LINE__, ##__VA_ARGS__); \
	}

#define SPTR_DEBUG_EXIT(fmt, ...) \
	if (DebugSharedPtr) { \
		printf("[%4d] -- " fmt "\n", __LINE__, ##__VA_ARGS__); \
	}

#define SPTR_PRINT(fmt, ...) \
	if (DebugSharedPtr) { \
		printf(fmt, ##__VA_ARGS__); \
	}

#define SPTR_VARIADIC_PRINT(fmt, data) \
	if (DebugSharedPtr) { \
		EXPAND(printf(fmt, data)); \
	}

#define SPTR_DUMP(fmt, ...) \
	if (DebugSharedPtr) { \
		printf(COLOR_YELLOW fmt COLOR_NONE "\n", ##__VA_ARGS__); \
	}

#define SPTR_PTR COLOR_RED "%p" COLOR_NONE
#define SPTR_LONG COLOR_RED "%lu" COLOR_NONE
#define SPTR_INT COLOR_RED "%d" COLOR_NONE
#define SPTR_TYPE(...) COLOR_CYAN "%s" __VA_ARGS__ COLOR_NONE
#define SPTR_HEAD_TEMPLATE(color, name, ...) \
	color name "<" SPTR_TYPE() color __VA_ARGS__ ">" COLOR_NONE

#define SPTR_HEAD(...) SPTR_HEAD_TEMPLATE(COLOR_GREEN, "CSharedPtr", __VA_ARGS__)
#define ENSPTR_HEAD(...) SPTR_HEAD_TEMPLATE(COLOR_NONE, "CEnableSharedPtr", __VA_ARGS__)
#define WPTR_HEAD(...) SPTR_HEAD_TEMPLATE(COLOR_BLUE, "CWeakPtr", __VA_ARGS__)
#define SBASE_HEAD(...) SPTR_HEAD_TEMPLATE(COLOR_PURPLE, "CSharedBase", __VA_ARGS__)

#define SHARED_PTR_CHECK() \
	if (nullptr == mBase) { \
		if (nullptr != mPtr) { \
			SPTR_ERROR(SPTR_HEAD() SPTR_PTR " mBase is %p but mPtr is %p", \
					   TYPE_NAME(T), this, mBase, mPtr); \
			throw ES("mBase is nullptr but mPtr is not nullptr"); \
		} \
	} else { \
		if (nullptr == mPtr) { \
			SPTR_ERROR(SPTR_HEAD() SPTR_PTR " mBase is %p but mPtr is %p", \
					   TYPE_NAME(T), this, mBase, mPtr); \
			throw ES("mBase is not nullptr but mPtr is nullptr"); \
		} \
	}

#define SHARED_PTR_CHECK_DESTRUCTOR() \
	if (nullptr == mBase) { \
		if (nullptr != mPtr) { \
			SPTR_ERROR(SPTR_HEAD() SPTR_PTR " mBase is %p but mPtr is %p", \
					   TYPE_NAME(T), this, mBase, mPtr); \
			while (1); \
		} \
	} else { \
		if (nullptr == mPtr) { \
			SPTR_ERROR(SPTR_HEAD() SPTR_PTR " mBase is %p but mPtr is %p", \
					   TYPE_NAME(T), this, mBase, mPtr); \
			while (1); \
		} \
	}

#define WEAK_PTR_CHECK() \
	if (nullptr == mBase) { \
		if (nullptr != mPtr) { \
			SPTR_ERROR(WPTR_HEAD() SPTR_PTR " mBase is %p but mPtr is %p", \
					   TYPE_NAME(T), this, mBase, mPtr); \
			throw ES("mBase is nullptr but mPtr is not nullptr"); \
		} \
	} else { \
		if (nullptr == mPtr) { \
			SPTR_ERROR(WPTR_HEAD() SPTR_PTR " mBase is %p but mPtr is %p", \
					   TYPE_NAME(T), this, mBase, mPtr); \
			throw ES("mBase is not nullptr but mPtr is nullptr"); \
		} \
	}

#define WEAK_PTR_CHECK_DESTRUCTOR() \
	if (nullptr == mBase) { \
		if (nullptr != mPtr) { \
			SPTR_ERROR(WPTR_HEAD() SPTR_PTR " mBase is %p but mPtr is %p", \
					   TYPE_NAME(T), this, mBase, mPtr); \
			while (1); \
		} \
	} else { \
		if (nullptr == mPtr) { \
			SPTR_ERROR(WPTR_HEAD() SPTR_PTR " mBase is %p but mPtr is %p", \
					   TYPE_NAME(T), this, mBase, mPtr); \
			while (1); \
		} \
	}

#define SHARED_PTR_START_DEBUG() DebugSharedPtr = true
#define SHARED_PTR_STOP_DEBUG() DebugSharedPtr = false

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
#define SPTR_ERROR(fmt, ...)
#define SPTR_DEBUG_ENTRY(fmt, ...)
#define SPTR_DEBUG_EXIT(fmt, ...)
#define SPTR_PRINT(fmt, ...)
#define SPTR_VARIADIC_PRINT(fmt, data)
#define SPTR_DUMP(fmt, ...)
#define SPTR_HEAD
#define ENSPTR_HEAD(...)
#define WPTR_HEAD(...)
#define SBASE_HEAD(...)
#define SHARED_PTR_CHECK()
#define WEAK_PTR_CHECK()
#define SHARED_PTR_START_DEBUG()
#define SHARED_PTR_STOP_DEBUG()
class CSharedPtrEnableDebug
{
};

#endif /* DEBUG_SPTR */

#define DEBUG_TEMPLATE \
	class This = T
#define DECLARE_DEBUG_TEMPLATE \
	class This

#endif /* __SHARED_DEBUG_HPP__ */

