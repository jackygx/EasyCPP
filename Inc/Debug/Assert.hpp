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

#ifndef __ASSERT_HPP__
#define __ASSERT_HPP__

#define ENABLE_TRACE_ASSERT

/* Trace Assert */
#if defined(ENABLE_TRACE_ASSERT)
#define TRACE_ASSERT(cond, ...) \
	if (!(cond)) { \
		throw E("Assert fail: ", #cond, " | ", ##__VA_ARGS__); \
	}
#else
#define TRACE_ASSERT(cond, ...)
#endif /* ENABLE_TRACE_ASSERT */

/* Trace Check param */
#define CHECK_PARAM(cond, ...) \
	if (!(cond)) { \
		throw E("Invalid param: ", #cond, " | ", ##__VA_ARGS__); \
	}

#endif /* __ASSERT_HPP__ */

