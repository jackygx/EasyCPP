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

#ifndef __REGEX_HANDLER_RANGE_HPP__
#define __REGEX_HANDLER_RANGE_HPP__

#include <String/String.hpp>
#include "IRegexHandler.hpp"

DEFINE_CLASS(RegexRange);

/* Regex_type: 0-9, a-z, A-Z
 * Match the range.
 * 2 bytes member for range start and range end. */
class CRegexRange :
	public IRegexHandler
{
public:
	CRegexRange(char start, char end) :
		mStart(start),
		mEnd(end)
	{
		/* Does nothing */
	}

	virtual bool Match(const CStringPtr &str, uint32_t &idx)
	{
		char ch = str[idx++];
		REGEX_DEBUG("Matching %c in [%c-%c]\n", ch, mStart, mEnd);
		return ((ch >= mStart) && (ch <= mEnd));
	}

#ifdef DEBUG_REGEX
private:
	virtual void DoDebug(int) const
	{
		printf("CRegexRange: start: %c, end: %c\n", mStart, mEnd);
	}
#endif

private:
	char mStart;
	char mEnd;
};


#endif /* __REGEX_HANDLER_RANGE_HPP__ */

