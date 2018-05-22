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

#ifndef __REGEX_HANDLER_NORMAL_HPP__
#define __REGEX_HANDLER_NORMAL_HPP__

#include <String/String.hpp>
#include "IRegexHandler.hpp"

DEFINE_CLASS(RegexNormal);

/* Regex_type:
 * Exactly match a char.
 * 1 byte member for the match char */
class CRegexNormal :
	public IRegexHandler
{
public:
	CRegexNormal(char ch) :
		mCh(ch)
	{
		/* Does nothing */
	}

	virtual bool Match(const CStringPtr &str, uint32_t &idx)
	{
		REGEX_DEBUG("Matching %c vs %c\n", mCh, str[idx]);
		return (mCh == str[idx++]);
	}

#ifdef DEBUG_REGEX
private:
	virtual void DoDebug(int) const
	{
		printf("CRegexNormal: %c\n", mCh);
	}
#endif

private:
	char mCh;
};

#endif /* __REGEX_HANDLER_NORMAL_HPP__ */

