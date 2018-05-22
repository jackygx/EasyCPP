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

#include "CharSplit.hpp"
#include "LineSplit.hpp"
#include "TokenSplit.hpp"
#include "StringSplit.hpp"

CString::IteratorPtr CString::Split(const CConstStringPtr &str) const
{
	if (1 == str->GetSize()) {
		return CCharSplitIterPtr(Slice(0, -1), str[0]);
	} else {
		return CStringSplitIterPtr(Slice(0, -1), str);
	}
}

CString::IteratorPtr CString::SplitByToken(void) const
{
	return CTokenSplitIterPtr(Slice(0, -1));
}

CString::IteratorPtr CString::SplitByLine(void) const
{
	return CLineSplitIterPtr(Slice(0, -1));
}

