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

#ifndef __STRING_HEADER_HPP__
#define __STRING_HEADER_HPP__

#include <DataStruct/Iterator.hpp>
#include <DataStruct/Switchable.hpp>
#include <DataStruct/List.hpp>
#include <Meta/Is.hpp>
#include "StringParam.hpp"

DEFINE_CLASS(Json);
DEFINE_CLASS(String);
DEFINE_CLASS(StringArray);

class CString
{
private:
	CStringParam mData;

public:
	enum Order {
		NORMAL,
		REVERSE,
		BOTH,
	};

public:
	/* Create CString from parameters */
	template <class... Tn, ENABLE_IF(has_constructor<CStringParam, Tn...>)>
	inline CString(const Tn &... tn);

	/* Create CString from other CString(s) */
	template <class... Tn,
			 ENABLE_IF(!has_constructor<CStringParam, Tn...>),
			 ENABLE_IFS(has_constructor<CConstStringPtr, Tn>)>
	inline CString(const Tn &... tn);

	/* Basic feature */
	inline uint32_t GetSize(void) const;
	inline uint32_t GetCapacity(void) const;
	inline uint32_t GetOffset(void) const;

	inline void SetSize(uint32_t size);
	inline void SetOffset(uint32_t offset);

	/* Overload operation */
	inline void operator += (const CString &str);
	inline bool operator == (const CString &str) const;
	inline char &operator [] (uint32_t i);
	inline const char &operator [] (uint32_t i) const;

	inline bool operator > (const CString &str) const;
	inline bool operator < (const CString &str) const;

	template <class T>
	inline T Dump(uint32_t offset) const;

	template <typename T,
			 ENABLE_IF(std::is_pointer<T>),
			 ENABLE_IF(!IS_CONST(T))>
	inline T Convert(uint32_t offset = 0);

	template <typename T,
			 ENABLE_IF(std::is_pointer<T>),
			 ENABLE_IF(IS_CONST(T))>
	inline T Convert(uint32_t offset = 0) const;

	inline CStringPtr ToCStr(void) const;

	/* Memory operation */
	inline int Sprintf(const char *fmt, ...);
	inline void Memset(uint32_t offset, uint32_t size, uint8_t val);
	inline void Memcpy(const CString &str);
	inline void Memcpy(const CString &str, uint32_t offset, uint32_t size);
	inline CStringPtr Reverse(void) const;

	/* Remove the space/tab/return from begin or end */
	inline CStringPtr Strip(void) const;
	inline CStringPtr LStrip(void) const;
	inline CStringPtr RStrip(void) const;

	/* Start is included in the substring
	 * but end is NOT included */
	inline CStringPtr Slice(int start, int end) const;

	/* Find the given string
	 * Return the index in the string */
	inline int Find(char ch, Order order = NORMAL) const;
	inline int Find(const CConstStringPtr &str, Order order = NORMAL) const;

	/* Find the token. \bToken\b
	 * Return the index in the string */
	inline int FindToken(void) const;

	/* Switch/Case */
	DEFINE_SWITCHABLE(CString, CConstStringPtr);
	inline CStringSwitchPtr Switch(void) const;

public:
	/* Define Base String Iterator */
	DEFINE_ITERATOR(Iterator, CConstStringPtr);
	class Iterator :
		public IteratorBase
	{
		friend IteratorBase;
	protected:
		const CConstStringPtr mSrc;
		uint32_t mStart;
		uint32_t mEnd;
		bool mIsEnd;

	public:
		inline Iterator(const CConstStringPtr &src);
		virtual ~Iterator(void) {}

	private:
		virtual void _Begin(void) = 0;
		virtual void _Next(void) = 0;
		virtual void _Rest(void) = 0;
		virtual IteratorPtr _Reverse(void) = 0;

		inline bool _End(void) const;

		template <class Fn, class... Tn>
		inline decltype(auto) _Get(const Fn &fn, const Tn & ... tn);

	protected:
		inline void SetEnd(void);
	};

	/* Split string into string list */
	IteratorPtr Split(const CConstStringPtr &str) const;
	IteratorPtr SplitByToken(void) const;
	IteratorPtr SplitByLine(void) const;

	template <class Fn>
	inline void Match(const CStringPtr &regex, Fn fn) const;

	template <class T>
	friend void Debug(const T &);
	template <class T, class... Tn>
	friend void Debug(const T &, const Tn &...);

	template <class T>
	friend struct std::less;

	template <class T>
	friend class CException;

private:
	inline char *GetPtr(void);
	inline const char *GetPtr(void) const;
	inline uint32_t GetFree(void) const;

public:
	enum NumberFormat {
		FMT_HEX,
		FMT_DEC,
	};

	inline bool ToNum(int &val) const;
	inline int ToNum(enum NumberFormat format = FMT_DEC) const;
	inline bool IsNum(void) const;

public:
	CJsonPtr ToJson(void) const;

public:
	CStringPtr Base64Encode(void) const;
	CStringPtr Base64Decode(void) const;
};

#endif /* __STRING_HEADER_HPP__ */

