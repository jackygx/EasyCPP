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

	/* operator = from shared base */
	template <class T1,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
	inline CWeakPtr<T> &operator = (CSharedBase<T1> &base);
};

/* =====================================================================
 *							Implement CWeakPtr
 * ===================================================================== */
/* Constructor an empty weak pointer */
template <class T>
inline CWeakPtr<T>::CWeakPtr(void) :
	mBase(nullptr)
{
	SPTR_DEBUG("[CWeakPtr<%s>(%p)]: Construct default",
			TYPE_NAME(T), this);

	Dump();
}

/* Constructor from shared pointer */
template <class T>
template <class T1,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CWeakPtr<T>::CWeakPtr(const CSharedPtr<T1> &t) :
	mBase(nullptr)
{
	SPTR_DEBUG("++[CWeakPtr<%s>(%p)]: Construct from CSharedPtr<%s>",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (t) {
		mBase = t.mBase->template AddWeakRef<T>();
	}

	Dump();

	SPTR_DEBUG("--[CWeakPtr<%s>(%p)]: Construct from CSharedPtr<%s>",
			   TYPE_NAME(T), this, TYPE_NAME(T1));
}

/* Destructor */
template <class T>
inline CWeakPtr<T>::~CWeakPtr(void)
{
	SPTR_DEBUG("++[CWeakPtr<%s>(%p)]: Destruct", TYPE_NAME(T), this);
	Dump();
	Release();
	SPTR_DEBUG("--[CWeakPtr<%s>(%p)]: Destruct", TYPE_NAME(T), this);
}

/* operator = from shared pointer */
template <class T>
template <class T1,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CWeakPtr<T> &CWeakPtr<T>::operator = (const CSharedPtr<T1> &t)
{
	SPTR_DEBUG("++[CWeakPtr<%s>]: operator = from CSharedPtr<%s>",
			   TYPE_NAME(T), TYPE_NAME(T1));
	Dump();

	Release();

	if (t) {
		mBase = t.mBase->template AddWeakRef<T>();
	}

	Dump();
	SPTR_DEBUG("--[CWeakPtr<%s>]: operator = from CSharedPtr<%s>",
			   TYPE_NAME(T), TYPE_NAME(T1));

	return *this;
}

/* Get shared pointer from weak pointer */
template <class T>
inline CSharedPtr<T> CWeakPtr<T>::Lock(void) const
{
	SPTR_DEBUG("++[CWeakPtr<%s>]: Lock", TYPE_NAME(T));
	Dump();

	CSharedPtr<T> t(nullptr);

	if ((nullptr != mBase) && mBase->Lock()) {
		t.mBase = mBase;
	}

	Dump();
	SPTR_DEBUG("--[CWeakPtr<%s>]: Lock", TYPE_NAME(T));

	return t;
}

/* Release weak pointer */
template <class T>
inline void CWeakPtr<T>::Release(void)
{
	SPTR_DEBUG("++[CWeakPtr<%s>]: Release", TYPE_NAME(T));
	Dump();

	if (nullptr != mBase) {
		mBase->ReleaseWeakRef();
		mBase = nullptr;
	}

	Dump();
	SPTR_DEBUG("--[CWeakPtr<%s>]: Release", TYPE_NAME(T));
}

template <class T>
inline uint32_t CWeakPtr<T>::GetRef(void) const
{
	return (nullptr == mBase) ? 0 : mBase->mRef;
}

template <class T>
inline uint32_t CWeakPtr<T>::GetWeakRef(void) const
{
	return (nullptr == mBase) ? 0 : mBase->mWeakRef;
}

/* operator = from shared base */
template <class T>
template <class T1,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CWeakPtr<T> &CWeakPtr<T>::operator = (CSharedBase<T1> &base)
{
	SPTR_DEBUG("++[CWeakPtr<%s>]: operator = from CSharedBase<%s>",
			   TYPE_NAME(T), TYPE_NAME(T1));

	Release();

	mBase = base.template AddWeakRef<T>();

	SPTR_DEBUG("--[CWeakPtr<%s>]: operator = from CSharedBase<%s>",
			   TYPE_NAME(T), TYPE_NAME(T1));

	return *this;
}

template <class T>
inline void CWeakPtr<T>::Dump(void) const
{
	SPTR_DEBUG("[CWeakPtr<%s>(%p)]: mBase: %p, mPtr: %p, ref: %u, wref: %u",
			   TYPE_NAME(T), this, mBase, mBase ? mBase->Get() : nullptr,
			   GetRef(), GetWeakRef());
}

#endif /* __WEAK_PTR_HPP__ */

