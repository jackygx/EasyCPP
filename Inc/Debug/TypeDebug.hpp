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

#ifndef __TYPE_DEBUG_HPP__
#define __TYPE_DEBUG_HPP__

#include <Demangle.hpp>

#define TYPE_NAME_BUF_SIZE 500

template <class T>
class CTypeName
{
public:
	inline CTypeName(void) :
		mSize(0)
	{
		/* Does nothing */
	}

	inline const char *Get(void)
	{
		Demangle<T>([&](const char *name) {
			mSize += snprintf(&mBuf[mSize], TYPE_NAME_BUF_SIZE - mSize, "%s", name);
		});
		return mBuf;
	}

protected:
	uint32_t mSize;
	char mBuf[TYPE_NAME_BUF_SIZE];
};

#define DEFINE_PRE_TYPE(TYPE) \
	template <class T> \
	class CTypeName<TYPE T> : \
		public CTypeName<T> \
	{ \
	public: \
		inline const char *Get(void) \
		{ \
			CTypeName<T>::mSize += snprintf( \
				&CTypeName<T>::mBuf[CTypeName<T>::mSize], \
				TYPE_NAME_BUF_SIZE - CTypeName<T>::mSize, #TYPE " "); \
			return CTypeName<T>::Get(); \
		} \
	}

DEFINE_PRE_TYPE(const);
DEFINE_PRE_TYPE(volatile);

#define DEFINE_POST_TYPE(TYPE) \
	template <class T> \
	class CTypeName<T TYPE> : \
		public CTypeName<T> \
	{ \
	public: \
		inline const char *Get(void) \
		{ \
			CTypeName<T>::Get(); \
			CTypeName<T>::mSize += snprintf( \
				&CTypeName<T>::mBuf[CTypeName<T>::mSize], \
				TYPE_NAME_BUF_SIZE - CTypeName<T>::mSize, " " #TYPE); \
			return CTypeName<T>::mBuf; \
		} \
	}

DEFINE_POST_TYPE(&);
DEFINE_POST_TYPE(&&);
DEFINE_POST_TYPE(*);

#define TYPE_NAME(T) CTypeName<T>().Get()

#endif /* __TYPE_DEBUG_HPP__ */

