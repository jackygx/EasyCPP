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

#ifndef __DEBUG_HPP__
#define __DEBUG_HPP__

#include <stdio.h>

#include <String/String.hpp>

#include "Assert.hpp"

#define __NULL__        (0)         /* 0   */
#define __TRACE__       (0x1 << 0)  /* 1   */
#define __DEBUG__       (0x1 << 1)  /* 2   */
#define __INFO__        (0x1 << 2)  /* 4   */
#define __WARNING__     (0x1 << 3)  /* 8   */
#define __ERROR__       (0x1 << 4)  /* 16  */
#define __FATAL__       (0x1 << 5)  /* 32  */
#define __MEMDUMP__     (0x1 << 6)  /* 64  */
#define __ASSERT__      (0x1 << 7)  /* 128 */
#define __CONSTRUCT__   (0x1 << 8)  /* 256 */
#define __ALL__         (0xFFFF)

/* Trace Error */
#if defined(ENABLE_TRACE_ERROR)
#define TRACE_ERROR(...) \
	::Debug(__VA_ARGS__)
#else
#define TRACE_ERROR(msg, ...)
#endif /* ENABLE_TRACE_ERROR */

/* Trace Debug */
#if defined(ENABLE_TRACE_DEBUG)
#define TRACE_DEBUG(...) \
	::Debug(__VA_ARGS__)
#else
#define TRACE_DEBUG(msg, ...)
#endif /* ENABLE_TRACE_DEBUG */

/* Trace Info */
#if defined(ENABLE_TRACE_INFO)
#define TRACE_INFO(...) \
	::Debug(__VA_ARGS__)
#else
#define TRACE_INFO(msg, ...)
#endif /* ENABLE_TRACE_INFO */

/* Trace Info */
#if defined(ENABLE_TRACE_DUMP)
#define TRACE_DUMP(str, ...) \
	::DebugDump(str, ##__VA_ARGS__)
#else
#define TRACE_DUMP(str, ...)
#endif /* ENABLE_TRACE_INFO */

inline void PutChar(const CConstStringPtr &buf)
{
	for (uint32_t i = 0; i < buf->GetSize(); ++i)
		putchar(buf[i]);
}

template <class... Tn>
inline void Debug(const Tn &... tn)
{
	EXPAND(PutChar(CConstStringPtr(tn)));
}

void DebugDump(const CConstStringPtr &str,
			   uint32_t shead = 32, uint32_t stail = 0);

#endif /* __DEBUG_HPP__ */

