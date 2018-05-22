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

#ifndef __REGEX_HANDLER_OR_HPP__
#define __REGEX_HANDLER_OR_HPP__

#include <String/String.hpp>
#include "IRegexHandler.hpp"

DEFINE_CLASS(RegexOr);

/* Regex_type: |
 * Any sub regex match = match.
 * sub saves the regex any of which should pass the match.
 * No member */
class CRegexOr :
	public IRegexHandler
{
public:
	virtual bool Match(const CStringPtr &str, uint32_t &idx)
	{
		uint32_t t = idx;
		REGEX_DEBUG("Matching or\n");

		for (auto it = mSub; it; it = it->mNext) {
			if (it->Match(str, idx)) {
				return true;
			}
		}

		idx = t;
		return false;
	}

#ifdef DEBUG_REGEX
private:
	virtual void DoDebug(int depth) const
	{
		printf("CRegexOr\n");

		if (mSub)
			mSub->Debug(depth + 2);
	}
#endif

private:
	IRegexHandlerPtr mSub;
};

#endif /* __REGEX_HANDLER_OR_HPP__ */

