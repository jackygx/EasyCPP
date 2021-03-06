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

#ifndef __REGEX_HANDLER_NUMBER_HPP__
#define __REGEX_HANDLER_NUMBER_HPP__

#include <String/String.hpp>
#include "IRegexHandler.hpp"

DEFINE_CLASS(RegexNumber);

/* Regex_type: \d
 * Match only number.
 * No member */
class CRegexNumber :
	public IRegexHandler
{
public:
	virtual bool Match(const CStringPtr &str, uint32_t &idx)
	{
		char ch = str[idx++];
		REGEX_DEBUG("Matching number: %c\n", ch);
		return ((ch <= '9') && (ch >= '0'));
	}

#ifdef DEBUG_REGEX
private:
	virtual void DoDebug(int) const
	{
		printf("CRegexNumber\n");
	}
#endif
};

#endif /* __REGEX_HANDLER_NUMBER_HPP__ */

