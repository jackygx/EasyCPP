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

#include <String/StrArray.hpp>
#include <String/Str.hpp>


CStrArray::CStrArray(void) :
	mSize(0)
{
	/* Does nothing */
}

CStrArray::~CStrArray(void)
{
	/* Does nothing */
}

bool CStrArray::ForEach(const StrArrayIterCb &cb)
{
	for (auto it = mStrs.Begin(); it != mStrs.End(); ++it) {
		if (!cb(*it))
			return false;
	}

	return true;
}

CStrPtr CStrArray::Join(void) const
{
	CStrPtr str(CreateString(mSize));

	for (auto it = mStrs.Begin(); it != mStrs.End(); ++it)
		str += *it;

	return str;
}

void CStrArray::Log(const ILoggerPtr &logger) const
{
	for (auto it = mStrs.Begin(); it != mStrs.End(); ++it)
		(*it)->Log(logger);
}

