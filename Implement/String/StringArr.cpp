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

#include <String/String.hpp>
#include <String/StringArr.hpp>

CStringPtr CStringArr::Join(void) const
{
	if (mSize >= DEFAULT_STR_SIZE) {
		return DoJoin(CMemPtr(new char[mSize], CMemPtr::ArrayDeleter), mSize + 1);
	}else {
		return DoJoin(StringBufPool::Alloc(), DEFAULT_STR_SIZE);
	}
}

CStringPtr CStringArr::DoJoin(const CMemPtr &mem, uint32_t capacity) const
{
	CStringPtr str(mem, capacity);
	char *buf = str->GetPtr();
	uint32_t size = 0;

	for (uint32_t i = 0; i < mStrs.size(); ++i) {
		CStringPtr tmp(mStrs[i]);
		::memcpy(&buf[size], tmp->GetPtr(), tmp->GetSize());
		size += tmp->GetSize();
	}

	buf[size] = '\0';

	CHECK_PARAM(size == mSize, "Illegal size to join: %d vs %d", size, mSize);

	str->mSize = size;

	return str;
}

void CStringArr::operator += (const CStringPtr &str)
{
	mStrs.push_back(str);
	mSize += str->GetSize();
}

void CStringArr::operator += (const CStringArrPtr &arr)
{
	std::vector<CStringPtr> &strs = arr->mStrs;

	for (uint32_t i = 0; i < strs.size(); ++i) {
		mStrs.push_back(strs[i]);
	}

	mSize += arr->mSize;
}

void CStringArr::Log(const IDebugPtr &debug) const
{
	for (uint32_t i = 0; i < mStrs.size(); ++i)
		mStrs[i]->Log(debug);
}

