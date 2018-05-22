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

#ifndef __IREGEX_HANDLER_HPP__
#define __IREGEX_HANDLER_HPP__

#include <Interface/Interface.hpp>
#include <Debug/Debug.hpp>

#define DEBUG_REGEX

#if 0
#define REGEX_DEBUG(fmt, ...) printf(fmt, ##__VA_ARGS__);
#else
#define REGEX_DEBUG(fmt, ...)
#endif

DEFINE_CLASS(String);
DEFINE_INTERFACE(RegexHandler);

class IRegexHandler
{
public:
	inline IRegexHandler(void) :
		mNext(nullptr)
	{
		/* Does nothing */
	}

	virtual ~IRegexHandler(void)
	{
		/* Does nothing */
	}

	virtual bool Match(const CStringPtr &str, uint32_t &idx) = 0;

	virtual bool IsMulti(void) const
	{
		return false;
	}

	virtual bool IsAssert(void) const
	{
		return false;
	}

#ifdef DEBUG_REGEX
	inline void Debug(int depth) const
	{
		for (int i = 0; i < depth; ++i) {
			printf(" ");
		}

		DoDebug(depth);

		if (mNext)
			mNext->Debug(depth);
	}
#endif

#ifdef DEBUG_REGEX
private:
	virtual void DoDebug(int depth) const = 0;
#endif

public:
	IRegexHandlerPtr mNext;
};

#endif /* __IREGEX_HANDLER_HPP__ */

