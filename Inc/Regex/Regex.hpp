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

#ifndef __REGEX_HPP__
#define __REGEX_HPP__

#include <Interface/Interface.hpp>
#include <String/StringHeader.hpp>

#include "IRegexHandler.hpp"
#include "RegexHandlerGroup.hpp"

DEFINE_CLASS(Regex);

class CRegex
{
public:
	inline CRegex(void);

	void Compile(const CStringPtr &reg);

	template <class Fn>
	void Match(const CStringPtr &str, Fn fn);

#ifdef DEBUG_REGEX
	inline void Debug(void) const
	{
		if (mRoot)
			mRoot->Debug(0);
		else
			printf("Empty regex\n");
	}
#endif

private:
	IRegexHandlerPtr CreateHGroup(const CStringPtr &reg, uint32_t &i);
	IRegexHandlerPtr CreateHandler(const CStringPtr &reg, uint32_t &i);
	IRegexHandlerPtr CreateHBS(const CStringPtr &reg, uint32_t &i);
	IRegexHandlerPtr CreateHMul(const CStringPtr &reg, uint32_t &i);
	CRegexGroupPtr CreateGroup(const CStringPtr &reg, uint32_t &i);

private:
	CRegexGroupPtr mRoot;
	uint32_t mGroupNum;
};

inline CRegex::CRegex(void) :
	mGroupNum(0)
{
	/* Does nothing */
}

template <class Fn>
void CRegex::Match(const CStringPtr &str, Fn fn)
{
	CHECK_PARAM(str, "input str is null");
	TRACE_ASSERT(mRoot, "mRoot is null???");

	CList<CConstStringPtr> arr;

	for (uint32_t idx = 0; idx < str->GetSize();) {
		uint32_t i = 0;
		auto _str(str->Slice(idx, -1));

		if (mRoot->Match(_str, i)) {
			CRegexGroupPtr iter(mRoot);

			do {
				arr.PushBack(_str->Slice(iter->GetGroupStart(),
										 iter->GetGroupEnd()));
				iter = iter->NextGroup();
			} while (iter);

			fn(arr);
			idx += mRoot->GetGroupEnd();
		} else {
			++idx;
		}
	}
}

#endif /* __REGEX_HPP__ */

