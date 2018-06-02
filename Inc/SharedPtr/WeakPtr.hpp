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

#ifndef __WEAK_PTR_HPP__
#define __WEAK_PTR_HPP__

#include "SharedBase.hpp"

template <class T>
class CSharedPtr;

template <class T>
class CWeakPtr
{
private:
	/* Object */
	T *mPtr;

	/* Saves counter and deleter */
	CSharedBase<T> *mBase;

public:
	/* Constructor an empty weak pointer */
	inline CWeakPtr(void);

	/* Constructor from shared pointer */
	template <class T1,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
	inline CWeakPtr(const CSharedPtr<T1> &t);

	/* Destructor */
	inline ~CWeakPtr(void);

	/* operator = from shared pointer */
	template <class T1,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
	inline CWeakPtr<T> &operator = (const CSharedPtr<T1> &t);

	/* Get shared pointer from weak pointer */
	inline CSharedPtr<T> Lock(void) const;

	/* Release weak pointer */
	inline void Release(void);

	/* Get reference information */
	inline uint32_t GetRef(void) const;
	inline uint32_t GetWeakRef(void) const;

private: /* For debug purpose only. */
	inline void Dump(void) const;

	/* This is only allowed used by CEnableSharedPtr<T> to SetShared() */
private:
	template <class T1>
	friend class CEnableSharedPtr;
};

/* =====================================================================
 *							Implement CWeakPtr
 * ===================================================================== */
/* Constructor an empty weak pointer */
template <class T>
inline CWeakPtr<T>::CWeakPtr(void) :
	mPtr(nullptr),
	mBase(nullptr)
{
	WEAK_PTR_CHECK();
	SPTR_DEBUG(WPTR_HEAD() SPTR_PTR " Construct default",
			TYPE_NAME(T), this);

	Dump();
}

/* Constructor from shared pointer */
template <class T>
template <class T1,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CWeakPtr<T>::CWeakPtr(const CSharedPtr<T1> &t) :
	mPtr(nullptr),
	mBase(nullptr)
{
	WEAK_PTR_CHECK();
	SPTR_DEBUG_ENTRY(WPTR_HEAD() SPTR_PTR " Construct from " WPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t);

	if (t) {
		mPtr = DynamicCast<T1, T>(t.mPtr);
		mBase = t.mBase->template AddWeakRef<T>();
	}

	Dump();

	SPTR_DEBUG_EXIT(WPTR_HEAD() SPTR_PTR " Construct from " WPTR_HEAD() SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T1), &t);
}

/* Destructor */
template <class T>
inline CWeakPtr<T>::~CWeakPtr(void)
{
	WEAK_PTR_CHECK_DESTRUCTOR();
	SPTR_DEBUG_ENTRY(WPTR_HEAD() SPTR_PTR " Destruct", TYPE_NAME(T), this);
	Dump();
	Release();
	SPTR_DEBUG_EXIT(WPTR_HEAD() SPTR_PTR " Destruct", TYPE_NAME(T), this);
}

/* operator = from shared pointer */
template <class T>
template <class T1,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CWeakPtr<T> &CWeakPtr<T>::operator = (const CSharedPtr<T1> &t)
{
	WEAK_PTR_CHECK();
	SPTR_DEBUG_ENTRY(WPTR_HEAD() SPTR_PTR " =& " WPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t);
	Release();

	if (t) {
		mPtr = DynamicCast<T1, T>(t.mPtr);
		mBase = t.mBase->template AddWeakRef<T>();

		Dump();
	}

	SPTR_DEBUG_EXIT(WPTR_HEAD() SPTR_PTR " =& " WPTR_HEAD() SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T1), &t);

	return *this;
}

/* Get shared pointer from weak pointer */
template <class T>
inline CSharedPtr<T> CWeakPtr<T>::Lock(void) const
{
	WEAK_PTR_CHECK();
	SPTR_DEBUG_ENTRY(WPTR_HEAD() SPTR_PTR " Lock", TYPE_NAME(T), this);

	if ((nullptr != mBase) && mBase->Lock()) {

		CSharedPtr<T> t(mPtr, mBase);

		SPTR_DEBUG_EXIT(WPTR_HEAD() SPTR_PTR " Lock" SPTR_HEAD() SPTR_PTR,
						TYPE_NAME(T), this, TYPE_NAME(T), &t);

		return t;

	} else {
		SPTR_DEBUG_ENTRY(WPTR_HEAD() SPTR_PTR " Lock empty", TYPE_NAME(T), this);

		return CSharedPtr<T>(nullptr);
	}
}

/* Release weak pointer */
template <class T>
inline void CWeakPtr<T>::Release(void)
{
	WEAK_PTR_CHECK();
	SPTR_DEBUG_ENTRY(WPTR_HEAD() SPTR_PTR " Release", TYPE_NAME(T), this);

	if (nullptr != mBase) {
		mBase->ReleaseWeakRef();
		mBase = nullptr;
		mPtr = nullptr;
	}

	SPTR_DEBUG_EXIT(WPTR_HEAD() SPTR_PTR " Release", TYPE_NAME(T), this);
}

template <class T>
inline uint32_t CWeakPtr<T>::GetRef(void) const
{
	WEAK_PTR_CHECK();
	return (nullptr == mBase) ? 0 : mBase->mRef;
}

template <class T>
inline uint32_t CWeakPtr<T>::GetWeakRef(void) const
{
	WEAK_PTR_CHECK();
	return (nullptr == mBase) ? 0 : mBase->mWeakRef;
}

template <class T>
inline void CWeakPtr<T>::Dump(void) const
{
	WEAK_PTR_CHECK();
	SPTR_DUMP("Addr: %p Base: %p Ptr: %p ref: %d wref: %d Type: %s",
			  this, mBase, mPtr, GetRef(), GetWeakRef(), TYPE_NAME(T));
}

#endif /* __WEAK_PTR_HPP__ */

