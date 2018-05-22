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

#ifndef __REGEX_HANDLER_GROUP_HPP__
#define __REGEX_HANDLER_GROUP_HPP__

#include <DataStruct/List.hpp>
#include "IRegexHandler.hpp"

DEFINE_CLASS(RegexGroup);

typedef CList<IRegexHandlerPtr> HandlerStack;

/* Regex_type: ()
 * All the sub regex match = match.
 * sub saves the regex all of which should pass the match.
 * No member */
class CRegexGroup :
	public IRegexHandler
{
public:
	inline CRegexGroup(void) :
		mSub(nullptr),
		mNextGroup(nullptr),
		mGroupStart(0),
		mGroupEnd(0)
	{
		/* Does nothing */
	}

	inline CRegexGroup(const IRegexHandlerPtr &sub) :
		mSub(sub),
		mNextGroup(nullptr),
		mGroupStart(0),
		mGroupEnd(0)
	{
		/* Does nothing */
	}

	virtual bool Match(const CStringPtr &str, uint32_t &idx);

	inline void SetSub(const IRegexHandlerPtr &sub)
	{
		mSub = sub;
	}

	inline uint16_t GetGroupStart(void) const
	{
		return mGroupStart;
	}

	inline uint16_t GetGroupEnd(void) const
	{
		return mGroupEnd;
	}

	inline void SetNextGroup(const CRegexGroupPtr &nextGroup)
	{
		mNextGroup = nextGroup;
	}

	inline CRegexGroupPtr NextGroup(void) const
	{
		return mNextGroup;
	}

#ifdef DEBUG_REGEX
private:
	virtual void DoDebug(int depth) const
	{
		printf("CRegexGroup: start: %d, end: %d, stack depth: %d\n",
			   mGroupStart, mGroupEnd, mStack.GetSize());

		if (mSub)
			mSub->Debug(depth + 2);
	}
#endif

private:
	IRegexHandlerPtr mSub;
	CRegexGroupPtr mNextGroup;
	uint16_t mGroupStart;
	uint16_t mGroupEnd;
	HandlerStack mStack;
};

#endif /* __REGEX_HANDLER_GROUP_HPP__ */

