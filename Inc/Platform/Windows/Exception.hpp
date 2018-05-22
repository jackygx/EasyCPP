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

#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <DbgHelp.h>

#include <Common/Typedef.hpp>
#include <Meta/Expander.hpp>

#define EXCEPTION_BUF_SIZE 512
#define EXCEPTION_STACK_SIZE 20

template <class T>
class CSharedPtr;

class CString;

template <class T>
class CException
{
public:
	template <class... Tn>
	inline CException(const Tn &... tn)
	{
		EXPAND(mBuf += CSharedPtr<const T>(tn));
		InitSymbol();
		mStackSize = CaptureStackBackTrace(0, EXCEPTION_STACK_SIZE, mStack, NULL);
	}

	inline void Show(void) const
	{
		const char *buf = mBuf->GetPtr();
		for (uint32_t i = 0; i < mBuf->GetSize(); ++i) {
			putchar(buf[i]);
		}
		putchar('\n');

		HANDLE process= GetCurrentProcess();
		DWORD64 dwDisplacement = 0;

		char _symbol[sizeof(SYMBOL_INFO) + EXCEPTION_BUF_SIZE];
		SYMBOL_INFO *symbol = (SYMBOL_INFO *)_symbol;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = EXCEPTION_BUF_SIZE;

		printf("==========================\n");
		for (uint32_t i = 0; i < mStackSize; ++i) {
			SymFromAddr(process, (DWORD64)mStack[i], &dwDisplacement, symbol);
			printf("%d: 0x%llX - %s\n", i, symbol->Address, symbol->Name);
		}
		printf("==========================\n");
	}

private:
	inline void InitSymbol(void)
	{
		static bool bInited = false;

		if (!bInited) {
			SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
			SymInitialize(GetCurrentProcess(), NULL, TRUE);
			bInited = true;
		}
	}

private:
	void *mStack[EXCEPTION_STACK_SIZE];
	uint32_t mStackSize;
	CSharedPtr<T> mBuf;
};

typedef CSharedPtr<CException<CString>> CExceptionPtr;

#define E(...) \
	CExceptionPtr(new CException<CString>(__VA_ARGS__, " | ", __func__, " ", __LINE__)) \

#endif /* __EXCEPTION_HPP__ */

