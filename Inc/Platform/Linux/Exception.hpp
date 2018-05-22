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

#ifndef __EXCEPTION_CSTRING_HPP__
#define __EXCEPTION_CSTRING_HPP__

#include <stdarg.h>
#include <stdio.h>
#include <execinfo.h>
#include <unistd.h>

#include <Interface/Exception/IException.hpp>
#include <Common/Typedef.hpp>
#include <Meta/Expander.hpp>

#define EXCEPTION_BUF_SIZE 512
#define EXCEPTION_STACK_SIZE 20

template <class T>
class CSharedPtr;

class CString;

template <class T>
class CException :
	public IException
{
public:
	template <class... Tn>
	inline CException(const char *func, int line, const Tn &... tn) :
		mFunc(func),
		mLine(line)
	{
		EXPAND(mBuf += CSharedPtr<const T>(tn));
		mStackSize = backtrace(mStack, EXCEPTION_STACK_SIZE);
	}

	inline void Show(void) const
	{
		const char *buf = mBuf->GetPtr();
		for (uint32_t i = 0; i < mBuf->GetSize(); ++i) {
			putchar(buf[i]);
		}
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
	void *mStack[EXCEPTION_STACK_SIZE];
	uint32_t mStackSize;
	const char *mErr;
	CSharedPtr<T> mBuf;
};

#define E(...) \
	CException<CString>(__func__, __LINE__, __VA_ARGS__)

#endif /* __EXCEPTION_CSTRING_HPP__ */

