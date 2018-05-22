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

#ifndef __REGEX_HANDLER_MULTI_HPP__
#define __REGEX_HANDLER_MULTI_HPP__

#include <String/String.hpp>
#include "IRegexHandler.hpp"

DEFINE_CLASS(RegexMulti);

/* Regex_type: *, +, {2+}, {2}, {2, 10}
 * sub saves the regex to be matched multiple times.
 * 2 words for minMatchCnt and maxMatchCnt */
class CRegexMulti :
	public IRegexHandler
{
public:
	CRegexMulti(uint16_t minMatchCnt, uint16_t maxMatchCnt) :
		mMinMatchCnt(minMatchCnt),
		mMaxMatchCnt(maxMatchCnt),
		mCnt(0),
		mIdx(0)
	{
		/* Does nothing */
	}

	/* First round only Check the minimal requirement.
	 * After that, it will be "PUSH" into the stack.
	 * On any failure, it will be "POP" and re-matched. */
	virtual bool Match(const CStringPtr &str, uint32_t &idx)
	{
		mIdx = idx;

		for (mCnt = 0; mCnt < mMinMatchCnt; ++mCnt) {
			/* Match fail */
			if (!mSub->Match(str, idx) ||
			/* End of string */
				(str->GetSize() == idx)) {
				idx = mIdx;
				REGEX_DEBUG("Matching multi fail\n");
				return false;
			}
		}

		mIdx = idx;
		REGEX_DEBUG("Matching multi succeed\n");
		return true;
	}

	virtual bool IsMulti(void) const
	{
		return true;
	}

	inline void SetSub(const IRegexHandlerPtr &sub)
	{
		mSub = sub;
	}

	inline bool ReMatch(const CStringPtr &str, uint32_t &idx)
	{
		REGEX_DEBUG("Rematching multi: %d\n", mCnt);

		/* Exceed the max count limitation */
		if (mCnt++ == mMaxMatchCnt) {
			return false;
		}

		if (!mSub->Match(str, mIdx) ||
			str->GetSize() == mIdx) {
			return false;
		} else {
			idx = mIdx;
			return true;
		}
	}

#ifdef DEBUG_REGEX
private:
	virtual void DoDebug(int depth) const
	{
		printf("CRegexHandlerMulti: mMinMatchCnt: %d, "
			   "mMaxMatchCnt: %d, mCnt: %d, mIdx: %d\n",
			   mMinMatchCnt, mMaxMatchCnt, mCnt, mIdx);

		if (mSub)
			mSub->Debug(depth + 2);
	}
#endif

private:
	uint16_t mMinMatchCnt;
	uint16_t mMaxMatchCnt;
	uint16_t mCnt;
	uint32_t mIdx;
	IRegexHandlerPtr mSub;
};

#endif /* __REGEX_HANDLER_MULTI_HPP__ */

