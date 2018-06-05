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

#ifndef __EXCEPTION_SIMPLE_HPP__
#define __EXCEPTION_SIMPLE_HPP__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <unistd.h>

#include <Common/Typedef.hpp>
#include <Interface/Exception/IException.hpp>

#define EXCEPTION_STACK_SIZE 20

class CExceptionSimple :
	public IException
{
public:
	CExceptionSimple(const char *func, int line, const char *err) :
		mFunc(func),
		mLine(line),
		mErr(err)
	{
		mStackSize = backtrace(mStack, EXCEPTION_STACK_SIZE);
	}

	virtual void Show(void) const
	{
		printf("%s\n", mErr);
		printf(" Func: %s, line: %d\n", mFunc, mLine);
		printf("==========================\n");
		if (NULL != GetSymbol()) {
			for (uint32_t i = 0; i < mStackSize; ++i) {
				pretty_backtrace(mStack[i]);
			}
		} else {
			backtrace_symbols_fd(mStack, mStackSize, STDERR_FILENO);
		}
		printf("==========================\n");
	}

private:
	inline void pretty_backtrace(void *addr) const
	{
		char cmd[64];
		snprintf(cmd, 64, "addr2line -e %s %p", GetSymbol(), addr);
		system(cmd);
	}

private:
	const char *mFunc;
	int mLine;
	const char *mErr;
	void *mStack[EXCEPTION_STACK_SIZE];
	uint32_t mStackSize;
};

#define ES(...) \
	new CExceptionSimple(__func__, __LINE__, __VA_ARGS__)

#endif /* __EXCEPTION_SIMPLE_HPP__ */

