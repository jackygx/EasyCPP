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

#ifndef __SHARED_BASE_HPP__
#define __SHARED_BASE_HPP__

#include <Function/FuncImpl.hpp>
#include <Platform/PlatformHeader.hpp>
#include <Pool/PoolBase.hpp>

#include "SharedDebug.hpp"
#include "SharedMeta.hpp"

template <class T>
class CSharedPtr;

template <class T>
class CWeakPtr;

template <class T>
class CSharedBase
{
private:
	typedef void (*ObjDelFn)(void *);
	typedef void (*BaseDelFn)(char *);

	friend class CSharedPtr<T>;
	friend class CWeakPtr<T>;

	/* Reference counter */
	mutable uint32_t mRef;
	/* Weak reference counter */
	mutable uint32_t mWeakRef;
	/* Function to delete the SharedBase */
	BaseDelFn mBaseDelFn;
	/* Function to delete the object */
	void *mObj;
	ObjDelFn mObjDelFn;

public:
	inline CSharedBase(const BaseDelFn &baseDelFn,
					   void *obj,
					   const ObjDelFn &objDelFn);

	template <class T1 = T,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T1, T))>
	inline CSharedBase<T1> *AddRef(void) const;

	inline void ReleaseRef(void);

	template <class T1 = T,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T1, T))>
	inline CSharedBase<T1> *AddWeakRef(void) const;

	inline void ReleaseWeakRef(void);

	inline bool Lock(void) const;

private:
	inline CSharedBase(CSharedBase &);
	inline CSharedBase(CSharedBase &&);
	inline CSharedBase *operator =(CSharedBase &);
	inline CSharedBase *operator =(CSharedBase &&);
};

/* =====================================================================
 *								Implement
 * ===================================================================== */
template <class T>
inline CSharedBase<T>::CSharedBase(const BaseDelFn &baseDelFn,
								   void *obj,
								   const ObjDelFn &objDelFn) :
	mRef(1),
	/* The WeakRef is inited to 1 for Ref:
	 * When the Ref is decreased to 0,
	 * this WeakRef will be decreased by 1.
	 */
	mWeakRef(1),
	mBaseDelFn(baseDelFn),
	mObj(obj),
	mObjDelFn(objDelFn)
{
	SPTR_DEBUG(SBASE_HEAD() SPTR_PTR " constructor",
			   TYPE_NAME(T), this);
}

template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T1, T))>
inline CSharedBase<T1> *CSharedBase<T>::AddRef(void) const
{
	auto ref = ATOMIC_ADD_AND_FETCH(&mRef, 1);

	SPTR_DEBUG(SBASE_HEAD() SPTR_PTR " Add Ref: "
			   SPTR_INT " => " SPTR_INT " " SBASE_HEAD(),
			   TYPE_NAME(T), this, ref - 1, ref, TYPE_NAME(T1));

	return (CSharedBase<T1> *)this;
}

template <class T>
inline void CSharedBase<T>::ReleaseRef(void)
{
	auto ref = ATOMIC_SUB_AND_FETCH(&mRef, 1);

	SPTR_DEBUG(SBASE_HEAD() SPTR_PTR " Release Ref: " SPTR_INT " => " SPTR_INT " ",
			   TYPE_NAME(T), this, ref + 1, ref);

	if (0 == ref) {
		mObjDelFn(mObj);
		ReleaseWeakRef();
	}
}

template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T1, T))>
inline CSharedBase<T1> *CSharedBase<T>::AddWeakRef(void) const
{
	auto ref = ATOMIC_ADD_AND_FETCH(&mWeakRef, 1);

	SPTR_DEBUG(SBASE_HEAD() SPTR_PTR " Add wRef: "
			   SPTR_INT " => " SPTR_INT " " SBASE_HEAD(),
			   TYPE_NAME(T), this, ref - 1, ref, TYPE_NAME(T1));

	return (CSharedBase<T1> *)this;
}

template <class T>
inline void CSharedBase<T>::ReleaseWeakRef(void)
{
	auto ref = ATOMIC_SUB_AND_FETCH(&mWeakRef, 1);

	SPTR_DEBUG(SBASE_HEAD() SPTR_PTR " Release Ref: " SPTR_INT " => " SPTR_INT " ",
			   TYPE_NAME(T), this, ref + 1, ref);

	if (0 == ref) {
		mBaseDelFn((char *)this);
	}
}

template <class T>
inline bool CSharedBase<T>::Lock(void) const
{
	SPTR_DEBUG_ENTRY(SBASE_HEAD() SPTR_PTR " lock", TYPE_NAME(T), this);

	/* The CAS makes sure the following atomic operation:
	 * if (0 != mRef) {
	 *		++mRef;
	 * }
	 */
	while (true) {
		uint32_t tmp = mRef;

		if (0 == tmp) {
			SPTR_DEBUG_EXIT(SBASE_HEAD() SPTR_PTR " lock fail", TYPE_NAME(T), this);
			return false;
		}

		if (ATOMIC_COMPARE_AND_SWAP(&mRef, tmp, tmp + 1)) {
			SPTR_DEBUG_EXIT(SBASE_HEAD() SPTR_PTR " lock ok", TYPE_NAME(T), this);
			return true;
		}
	}
}

#endif /* __SHARED_BASE_HPP__ */

