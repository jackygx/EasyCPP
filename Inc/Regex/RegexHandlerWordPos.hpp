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

#ifndef __REGEX_HANDLER_WORD_POS_HPP__
#define __REGEX_HANDLER_WORD_POS_HPP__

#include <String/String.hpp>
#include "IRegexHandler.hpp"

DEFINE_CLASS(RegexWordPos);

/* Regex_type: \b
 * Match the start or end of the word
 * No member */
class CRegexWordPos :
	public IRegexHandler
{
public:
	virtual bool Match(const CStringPtr &str, uint32_t &idx)
	{
		REGEX_DEBUG("Matching word position\n");

		if ((0 != idx) && (idx != str->GetSize())) {
			char ch1 = str[idx - 1];
			bool b1 = ((ch1 == '\r') || (ch1 == '\n') ||
					   (ch1 == ' ') || (ch1 == '\t'));

			char ch2 = str[idx];
			bool b2 = ((ch2 == '\r') || (ch2 == '\n') ||
					   (ch2 == ' ') || (ch2 == '\t'));

			return b1 ^ b2;
		} else {
			return true;
		}
	}

#ifdef DEBUG_REGEX
private:
	virtual void DoDebug(int) const
	{
		printf("CRegexWordPos\n");
	}
#endif
};

#endif /* __REGEX_HANDLER_WORD_POS_HPP__ */

