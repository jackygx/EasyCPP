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
#include <Regex/RegexHandlerAny.hpp>
#include <Regex/RegexHandlerAssert.hpp>
#include <Regex/RegexHandlerGroup.hpp>
#include <Regex/RegexHandlerLineEnd.hpp>
#include <Regex/RegexHandlerLineStart.hpp>
#include <Regex/RegexHandlerMulti.hpp>
#include <Regex/RegexHandlerNormal.hpp>
#include <Regex/RegexHandlerNumber.hpp>
#include <Regex/RegexHandlerOr.hpp>
#include <Regex/RegexHandlerRange.hpp>
#include <Regex/RegexHandlerReference.hpp>
#include <Regex/RegexHandlerReverse.hpp>
#include <Regex/RegexHandlerReverseAssert.hpp>
#include <Regex/RegexHandlerSpace.hpp>
#include <Regex/RegexHandlerWord.hpp>
#include <Regex/RegexHandlerWordPos.hpp>
#include <Regex/Regex.hpp>

void CRegex::Compile(const CStringPtr &reg)
{
	CHECK_PARAM(reg);
	uint32_t size = reg->GetSize();
	uint32_t i = 0;
	CHECK_PARAM(size > 0, "Empty regex is not allowed");

	IRegexHandlerPtr first(CreateHandler(reg, i));
	CHECK_PARAM(first, "Empty regex is not allowed");

	IRegexHandlerPtr *cur = &first->mNext;
	IRegexHandlerPtr *prev = &first;

	while (i < size) {
		IRegexHandlerPtr next(CreateHandler(reg, i));
		CHECK_PARAM(next, "Unbalance () found in the regex");

		if (next->IsMulti()) {
			CRegexMultiPtr(next)->SetSub(*prev);
			*prev = next;
			cur = &next->mNext;
		} else {
			prev = cur;
			(*cur) = next;
			cur = &next->mNext;
		}
	}

	++mGroupNum;

	mRoot->SetSub(first);
}

IRegexHandlerPtr CRegex::CreateHGroup(const CStringPtr &reg, uint32_t &i)
{
	IRegexHandlerPtr first(CreateHandler(reg, i));
	CHECK_PARAM(first, "Empty group is not allowed");

	IRegexHandlerPtr *cur = &first->mNext;
	IRegexHandlerPtr *prev = &first;

	++mGroupNum;

	while (true) {
		IRegexHandlerPtr next(CreateHandler(reg, i));
		if (!next) {
			return first;
		}

		if (next->IsMulti()) {
			CRegexMultiPtr(next)->SetSub(*prev);
			*prev = next;
			cur = &next->mNext;
		} else {
			prev = cur;
			(*cur) = next;
			cur = &next->mNext;
		}
	}
}

IRegexHandlerPtr CRegex::CreateHandler(const CStringPtr &reg, uint32_t &i)
{
	if (i == reg->GetSize()) {
		throw E("Incomplete regex: ", DEC(i));
	}

	char ch = reg[i++];;

	switch (ch) {
	case '\\':
		return CreateHBS(reg, i);

	case '.':
		return CRegexAnyPtr();

	case '*':
		return CRegexMultiPtr(0, uint16_t(-1));

	case '+':
		return CRegexMultiPtr(1, uint16_t(-1));

	case '?':
		return CRegexMultiPtr(0, 1);

	case '^':
		return CRegexLineStartPtr();

	case '$':
		return CRegexLineEndPtr();

	/* New group */
	case '(':
		return CreateGroup(reg, i);

	/* End of the group */
	case ')':
		return nullptr;

	case '{':
		return CreateHMul(reg, i);

	case '[':
//		*handler = CRegexOrPtr();
//		GetRangeHandler(&(*handler)->mSub, reg, i);
//		return true;

	default:
		return CRegexNormalPtr(ch);
	}
}

CRegexGroupPtr CRegex::CreateGroup(const CStringPtr &reg, uint32_t &i)
{
	CRegexGroupPtr group(CreateHGroup(reg, i));
	CRegexGroupPtr iter(mRoot);
	CRegexGroupPtr tmp(nullptr);

	while ((tmp = iter->NextGroup())) {
		iter = tmp;
	}

	iter->SetNextGroup(group);
	return group;
}

IRegexHandlerPtr CRegex::CreateHBS(const CStringPtr &reg, uint32_t &i)
{
	if (i == reg->GetSize()) {
		throw E("Incomplete regex at backslash: ", DEC(i));
	}

	char ch;

	switch ((ch = reg[i++])) {
	case 'w':
		return CRegexWordPtr();

	case 'W':
		return CRegexReversePtr(CRegexWordPtr());

	case 's':
		return CRegexSpacePtr();

	case 'S':
		return CRegexReversePtr(CRegexSpacePtr());

	case 'd':
		return CRegexNumberPtr();

	case 'D':
		return CRegexReversePtr(CRegexNumberPtr());

	case 'b':
		return CRegexWordPosPtr();

	case 'B':
		return CRegexReversePtr(CRegexWordPosPtr());

	case '(':
	case ')':
	case '{':
	case '}':
	case '[':
	case ']':
	case '\\':
	case '*':
	case '+':
	case '?':
	case ',':
	case '.':
	case '^':
	case '$':
		return CRegexNormalPtr(ch);

	default:
		throw E("Illegal using backslash: ", DEC(ch));
	}
}

IRegexHandlerPtr CRegex::CreateHMul(const CStringPtr &reg, uint32_t &i)
{
	uint16_t min = 0;
	uint16_t max = 0;
	uint32_t size = reg->GetSize();

	for (; i < size; ++i) {
		char ch = reg[i];

		if (ch >= '0' && ch <= '9') {
			min = min * 10 + (ch - '0');
		} else if (',' == ch) {
			break;
		} else if ('}' == ch) {
			/* {n} */
			return CRegexMultiPtr(min, min);
		} else if (' ' != ch && '\t' != ch) {
			throw E("Illegal format in multi range handler(min): ", DEC(ch));
		}
	}

	for (++i; i < size; ++i) {
		char ch = reg[i];

		/* {n,} */
		if (ch == '}') {
			return CRegexMultiPtr(min, uint16_t(-1));
		} else if (' ' != ch && '\t' != ch) {
			break;
		}
	}

	for (; i < size; ++i) {
		char ch = reg[i];

		if (ch >= '0' && ch <= '9') {
			max = max * 10 + (ch - '0');
		} else if ('}' == ch) {
			/* {n,m} */
			return CRegexMultiPtr(min, max);
		} else if (' ' != ch || '\t' != ch) {
			throw E("Illegal format in multi range handler(max): ", DEC(ch));
		}
	}

	throw E("Incomplete regex at range handler: ", DEC(i));
}

