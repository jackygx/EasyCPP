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

#ifndef __REGEX_HANDLER_REFERENCE_HPP__
#define __REGEX_HANDLER_REFERENCE_HPP__

#include <String/String.hpp>
#include "IRegexHandler.hpp"
#include "RegexHandlerGroup.hpp"

DEFINE_CLASS(RegexReference);

/* Regex_type: \1
 * Match a group result.
 * No member */
class CRegexReference :
	public IRegexHandler
{
public:
	inline CRegexReference(const CRegexGroupPtr &ref) :
		mRef(ref)
	{
		/* Does nothing */
	}

	virtual bool Match(const CStringPtr &str, uint32_t &idx)
	{
		REGEX_DEBUG("Matching reference\n");
		CStringPtr src(str->Slice(mRef->GetGroupStart(), mRef->GetGroupEnd()));
		CStringPtr tar(str->Slice(idx, -1));
		uint32_t ssrc = src->GetSize();
		uint32_t star = tar->GetSize();

		if (ssrc <= star) {
			tar = tar->Slice(0, ssrc);
			idx += ssrc;
			return src == tar;
		} else {
			idx += star;
			return false;
		}
	}

#ifdef DEBUG_REGEX
private:
	virtual void DoDebug(int) const
	{
		printf("CRegexReference\n");
	}
#endif

private:
	CRegexGroupPtr mRef;
};

#endif /* __REGEX_HANDLER_REFERENCE_HPP__ */

