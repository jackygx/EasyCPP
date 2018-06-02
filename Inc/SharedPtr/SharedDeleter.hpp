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

#ifndef __SHARED_DELETER_HPP__
#define __SHARED_DELETER_HPP__

#include <Meta/Meta.hpp>

template <class T>
class CSharedFuncPointerDeleter
{
private:
	typedef void (*Deleter)(T *);

	T *mPtr;
	Deleter mDeleter;

public:
	inline CSharedFuncPointerDeleter(T *ptr, Deleter deleter) :
		mPtr(ptr),
		mDeleter(deleter)
	{
		/* Does nothing */
	}

	inline static void Delete(void *_ptr)
	{
		auto deleter = (CSharedFuncPointerDeleter *)_ptr;
		deleter->DoDelete();
	}

private:
	inline void DoDelete(void)
	{
		mDeleter(mPtr);
	}
};

template <class T, class DeleterFn>
class CSharedLambdaDeleter
{
	typedef CLASS_TYPE(DeleterFn) LambdaType;

private:
	T *mPtr;
	LambdaType *mLambda;
	char *mBuf[sizeof(DeleterFn)];
	void (*mCall)(LambdaType *, T *);

public:
	inline CSharedLambdaDeleter(T *ptr, DeleterFn &deleter) :
		mPtr(ptr),
		mLambda(new (mBuf) LambdaType(deleter)),
		mCall([](LambdaType *lambda, T *ptr) {
			lambda->operator()(ptr);
			lambda->~LambdaType();
		})
	{
		/* Does nothing */
	}

	inline static void Delete(void *_ptr)
	{
		auto deleter = (CSharedLambdaDeleter *)_ptr;
		deleter->DoDelete();
	}

private:
	inline void DoDelete(void)
	{
		mCall(mLambda, mPtr);
	}
};

template <class T>
class CSharedDefaultDeleter
{
private:
	T *mPtr;

public:
	inline CSharedDefaultDeleter(T *ptr) :
		mPtr(ptr)
	{
		/* Does nothing */
	}

	inline static void Delete(void *_ptr)
	{
		auto deleter = (CSharedDefaultDeleter *)_ptr;
		deleter->DoDelete();
	}

private:
	inline void DoDelete(void)
	{
		mPtr->T::~T();
	}
};

#endif /* __SHARED_DELETER_HPP__ */

