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
#include <String/Json.hpp>

//#define DEBUG_JSON_PARSER

#ifdef DEBUG_JSON_PARSER
#define JSON_DEBUG(...) TRACE_INFO(__VA_ARGS__)
#else
#define JSON_DEBUG(...)
#endif

class CStringToJson
{
private:
	CConstStringPtr mStr;
	const char *mPtr;
	uint32_t mStart;
	uint32_t mEnd;

public:
	inline CStringToJson(CConstStringPtr &&str) :
		mStr(str),
		mPtr(str),
		mStart(0),
		mEnd(str->GetSize())
	{
		/* Does nothing */
	}

	inline CJsonPtr Parse(void)
	{
		JSON_DEBUG("Start parsing json: >>> ", mStr, " <<<\n");

		CJsonPtr root(nullptr, nullptr);

		if (!Parse(root)) {
			return CJsonPtr(nullptr, nullptr);
		}

		JSON_DEBUG("Json is Successfully parsed\n");

		return root;
	}

private:
#define JSON_ERROR(...) \
	do { \
		TRACE_INFO("[Json]", __VA_ARGS__, \
				   " mStart: ", DEC(mStart), \
				   " mEnd: ", DEC(mEnd), \
				   " mPtr[mStart]: >>> ", CHAR(mPtr[mStart]), " <<<\n"); \
		return false; \
	} while (0)

#define JSON_CHECK(cond, ...) \
	if (!(cond)) \
	{ \
		JSON_ERROR(__VA_ARGS__); \
	}

	inline bool IsSpace(char ch)
	{
		return ' ' == ch ||
			'\t' == ch ||
			'\r' == ch ||
			'\n' == ch;
	}

	inline bool IsNum(char ch)
	{
		return ch >= '0' && ch <= '9';
	}

	inline void Strip(void)
	{
		for (; mStart < mEnd; ++mStart) {
			if (!IsSpace(mPtr[mStart])) {
				return;
			}
		}
	}

	inline bool IsNextChar(char ch)
	{
		for (; mStart < mEnd; ++mStart) {
			char _ch = mPtr[mStart];

			if (!IsSpace(_ch)) {
				return _ch == ch;
			}
		}

		return false;
	}

	inline int Find(char ch)
	{
		for (uint32_t i = mStart; i < mEnd; ++i) {
			if (mPtr[i] == ch) {
				return (int)i;
			}
		}
		return -1;
	}

	inline bool ParseKey(CJsonPtr &node)
	{
		/* Key must start with >>> " <<< */
		JSON_CHECK(IsNextChar('"'), "key does not start with >>> \" <<<");

		/* mStart -> >>> " <<< */
		++mStart;

		int idx = Find('"');
		JSON_CHECK(idx > 0, "key does not ended with >>> \" <<<");

		JSON_DEBUG("Set key: >>> ", mStr->Slice(mStart, idx), " <<<\n");
		node->SetKey(mStr->Slice(mStart, idx));

		/* idx -> >>> " <<< */
		mStart = idx + 1;

		/* After the key, there should be a >>> : <<< */
		JSON_CHECK(IsNextChar(':'), "Key is not followed by >>> : <<<");

		/* mStart -> >>> : <<< */
		++mStart;

		return true;
	}

	inline void SetVal(CJsonPtr &node, uint32_t idx)
	{
		JSON_DEBUG("Set val: >>> ", mStr->Slice(mStart, idx), " <<<\n");
		node->SetVal(mStr->Slice(mStart, idx));
	}

	inline bool ParseVal(CJsonPtr &node)
	{
		Strip();

		int idx;
		char ch = mPtr[mStart];

		/* Parse value */
		switch (ch) {
		case '"':		/* value => "xxx" */
			/* mStart -> >>> : <<< */
			++mStart;
			idx = Find('"');
			JSON_CHECK(idx > 0, "value does not ended with >>> \" <<<");
			SetVal(node, idx);
			/* idx -> >>> " <<< */
			mStart = idx + 1;
			return true;

		case '{':		/* child => { xxx } */
			return DoParse(node, CJson::OBJECT);

		case '[':		/* Array => [ xxx ] */
			return DoParse(node, CJson::ARRAY);

		default:		/* value => number */
			if (IsNum(ch)) {
				for (uint32_t i = mStart + 1; i < mEnd; ++i) {
					if (!IsNum(mPtr[i])) {
						SetVal(node, i);
						/* i -> non-number */
						mStart = i;
						return true;
					}
				}
				JSON_ERROR("Value reaches end");
			} else {
				JSON_ERROR("Non-number value should be in >>> \" <<<");
			}
		}

		JSON_ERROR("I will not reach here!");
	}

	inline bool DoParse(CJsonPtr &root, CJson::Type type)
	{
		/* mStart => previous token */
		++mStart;

		JSON_DEBUG("Start parsing ", type == CJson::OBJECT? "object" : "array", EOS);

		while (true) {
			CJsonPtr child(nullptr, nullptr, type);
			root->AddChild(child);

			/* Only CJson::OBJECT has the keys */
			if (CJson::OBJECT == type) {
				JSON_CHECK(ParseKey(child), "Fail to parse key");
			}

			JSON_CHECK(ParseVal(child), "Fail to parse value");

			for (uint32_t i = mStart; i < mEnd; ++i) {
				char ch = mPtr[i];

				/* Next child */
				if (',' == ch) {
					mStart = i + 1;
					break;
				/* Ignore space */
				} else if (IsSpace(ch)) {
					/* Continue */
				/* Object finish */
				} else if (('}' == ch) && (type == CJson::OBJECT)) {
					JSON_DEBUG("Stop parsing object\n");
					mStart = i + 1;
					return true;
				} else if ((']' == ch) && (type == CJson::ARRAY)) {
					JSON_DEBUG("Stop parsing array\n");
					mStart = i + 1;
					return true;
				} else {
					JSON_ERROR("Unknown token");
				}
			}
		}
	}

	inline bool Parse(CJsonPtr &root)
	{
		JSON_CHECK(IsNextChar('{'), "Start >>> { <<< is not found");
		JSON_CHECK(DoParse(root, CJson::OBJECT), "Fail to parse");
		return true;
	}
};

CJsonPtr CString::ToJson(void) const
{
	return CStringToJson(Slice(0, -1)).Parse();
}

