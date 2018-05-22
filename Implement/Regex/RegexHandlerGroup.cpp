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

#include <EasyCpp.hpp>
#include <Regex/RegexHandlerGroup.hpp>
#include <Regex/RegexHandlerMulti.hpp>

bool CRegexGroup::Match(const CStringPtr &str, uint32_t &idx)
{
	uint32_t t = idx;
	auto it = mSub;

	mGroupStart = idx;

	/* To detect if the first element is an pre-assert */
	if (!it->Match(str, idx)) {
		idx = t;
		return false;
	}

	/* The pre-assert will not be recorded */
	if (it->IsAssert()) {
		mGroupStart = idx;
	/* Retriable handler will be push into the stack */
	} else if (it->IsMulti()) {
		REGEX_DEBUG("Push multi:\n");
		mStack.PushBack(it);
	}

	while ((it = it->mNext)) {
		uint32_t old = idx;

		if (!it->Match(str, idx)) {
			/* Check stack */
			if (mStack.GetSize() > 0) {
				/* Popup the last handler
				 * and re-match */
				mStack.PopBack(it);
				REGEX_DEBUG("Pop multi:\n");
				/* On succeed, push it again
				 * and continue the matching */
				CRegexMultiPtr h(it);
				if (h->ReMatch(str, idx)) {
					REGEX_DEBUG("Push multi:\n");
					mStack.PushBack(it);
					continue;
				}
			}

			idx = t;
			return false;
		}

		/* The post-assert will not be recorded
		 * and must be the last element */
		if (it->IsAssert()) {
			mGroupEnd = old;
			return true;
		} else if (it->IsMulti()) {
			REGEX_DEBUG("Push multi:\n");
			mStack.PushBack(it);
		}
	}

	mGroupEnd = idx;
	return true;
}

