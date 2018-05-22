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

#include <String/Json.hpp>

CJson::CJson(const CConstStringPtr &key, const CConstStringPtr &val, Type type) :
	mType(type),
	mKey(key),
	mVal(val),
	mChild(nullptr),
	mSibling(nullptr)
{
	/* Does nothing */
}

CJson::~CJson(void)
{
	/* Does nothing */
}

CConstStringPtr CJson::ToString(void) const
{
	CStringPtr str(STR(GetSize()));

	_ToString(str);

//	TRACE_DEBUG("Json size: ", size, " vs ", str->GetSize(), EOS);

	return str;
}

uint32_t CJson::GetSize(void) const
{
	uint32_t size = 0;

	if (mKey) {
		size += 3 + mKey->GetSize();
	}

	if (mVal) {
		size += mVal->GetSize();
		if (!mVal->IsNum())
			size += 2;
	} else if (mChild) {
		size += 2 + mChild->GetSize();
	}

	if (mSibling) {
		size += 1 + mSibling->GetSize();
	}

	return size;
}

void CJson::_ToString(CStringPtr &str) const
{
	if (mKey) {
		str += "\"";
		str += mKey;
		str += "\"";
		str += ":";
	}

	if (mVal) {
		if (mVal->IsNum()) {
			str += mVal;
		} else {
			str += "\"";
			str += mVal;
			str += "\"";
		}
	} else if (mChild) {
		switch (mChild->GetType()) {
		case OBJECT:
			str += "{";
			mChild->_ToString(str);
			str += "}";
			break;

		case ARRAY:
			str += "[";
			mChild->_ToString(str);
			str += "]";
			break;
		}
	}

	if (mSibling) {
		str += ",";
		mSibling->_ToString(str);
	}
}

void CJson::Dump(uint8_t lev) const
{
	for (int i = 0; i < lev; ++i)
		TRACE_INFO(" ");

	if (mKey) {
		TRACE_INFO("\"", mKey, "\" : ");
	}

	if (mVal) {
		TRACE_INFO("\"", mVal, "\"");
	} else {
		if (mChild) {
			switch (mChild->GetType()) {
			case OBJECT:
				TRACE_INFO("{\n");
				mChild->Dump(lev + 1);
				TRACE_INFO("}");
				break;

			case ARRAY:
				TRACE_INFO("[");
				mChild->Dump(lev + 1);
				TRACE_INFO("]");
				break;
			}
		}
	}

	if (mSibling) {
		TRACE_INFO(",\n");
		mSibling->Dump(lev);
	} else {
		TRACE_INFO("\n");
	}
}

