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
	typedef CFunc<void(T *)> ObjDelFn;
	typedef void (* BaseDelFn)(char *);

	friend class CSharedPtr<T>;
	friend class CWeakPtr<T>;

	/* Object */
	T *mPtr;
	/* Reference counter */
	mutable uint32_t mRef;
	/* Weak reference counter */
	mutable uint32_t mWeakRef;
	/* Function to delete the object */
	ObjDelFn mObjDelFn;
	/* Function to delete the SharedBase */
	BaseDelFn mBaseDelFn;

public:
	template <class T1,
			 class Fn,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
	inline CSharedBase(T1 *ptr, Fn &objDelFn, BaseDelFn baseDelFn);

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

	inline T *Get(void);

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
template <class T1,
		 class Fn,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CSharedBase<T>::CSharedBase(T1 *ptr, Fn &objDelFn, BaseDelFn baseDelFn) :
	mPtr(DynamicCast<T1, T>(ptr)),
	mRef(1),
	/* The WeakRef is inited to 1 for Ref:
	 * When the Ref is decreased to 0,
	 * this WeakRef will be decreased by 1.
	 */
	mWeakRef(1),
	mObjDelFn(objDelFn),
	mBaseDelFn(baseDelFn)
{
	SPTR_DEBUG("[CSharedBase<%s>(%p)]: construct from (%s)",
			   TYPE_NAME(T), this, TYPE_NAME(T1));
}

template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T1, T))>
inline CSharedBase<T1> *CSharedBase<T>::AddRef(void) const
{
	SPTR_DEBUG("++[CSharedBase<%s>(%p)]: before AddRef: %d. "
			   "Return CSharedBase<%s>",
			   TYPE_NAME(T), this, mRef, TYPE_NAME(T1));

	DynamicCast<T, T1>(mPtr);
	ATOMIC_ADD_AND_FETCH(&mRef, 1);

	SPTR_DEBUG("--[CSharedBase<%s>(%p)]: after  AddRef: %d. "
			   "Return CSharedBase<%s>",
			   TYPE_NAME(T), this, mRef, TYPE_NAME(T1));

	return (CSharedBase<T1> *)this;
}

template <class T>
inline void CSharedBase<T>::ReleaseRef(void)
{
	SPTR_DEBUG("++[CSharedBase<%s>(%p)]: before ReleaseRef: %d",
			   TYPE_NAME(T), this, mRef);

	if (0 == ATOMIC_SUB_AND_FETCH(&mRef, 1)) {
		mObjDelFn(mPtr);
		mPtr = nullptr;

		ReleaseWeakRef();
	}

	SPTR_DEBUG("--[CSharedBase<%s>(%p)]: after  ReleaseRef: %d",
			   TYPE_NAME(T), this, mRef);
}

template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T1, T))>
inline CSharedBase<T1> *CSharedBase<T>::AddWeakRef(void) const
{
	SPTR_DEBUG("++[CSharedBase<%s>(%p)]: before AddWeakRef: %d. "
			   "Return CSharedBase<%s>",
			   TYPE_NAME(T), this, mWeakRef, TYPE_NAME(T1));

	DynamicCast<T, T1>(mPtr);
	ATOMIC_ADD_AND_FETCH(&mWeakRef, 1);

	SPTR_DEBUG("--[CSharedBase<%s>(%p)]: after  AddWeakRef: %d. "
			   "Return CSharedBase<%s>",
			   TYPE_NAME(T), this, mWeakRef, TYPE_NAME(T1));

	return (CSharedBase<T1> *)this;
}

template <class T>
inline void CSharedBase<T>::ReleaseWeakRef(void)
{
	SPTR_DEBUG("++[CSharedBase<%s>(%p)]: before ReleaseWeakRef: %d",
			   TYPE_NAME(T), this, mWeakRef);

	if (0 == ATOMIC_SUB_AND_FETCH(&mWeakRef, 1)) {
		mBaseDelFn((char *)this);
	}

	SPTR_DEBUG("--[CSharedBase<%s>(%p)]: after  ReleaseWeakRef: %d",
			   TYPE_NAME(T), this, mWeakRef);
}

template <class T>
inline bool CSharedBase<T>::Lock(void) const
{
	SPTR_DEBUG("[CSharedBase<%s>(%p)]: lock", TYPE_NAME(T), this);

	/* The CAS makes sure the following atomic operation:
	 * if (0 != mRef) {
	 *		++mRef;
	 * }
	 */
	while (true) {
		uint32_t tmp = mRef;

		if (0 == tmp)
			return false;

		if (ATOMIC_COMPARE_AND_SWAP(&mRef, tmp, tmp + 1))
			return true;
	}
}

template <class T>
inline T *CSharedBase<T>::Get(void)
{
	return mPtr;
}

/* Help functions to create SharedBase */
/* Object is created by the caller.
 * Delete function should also be provided by the caller */
template <class T, class T1, class ObjDelFn>
static inline CSharedBase<T> *CreateSharedBase(T1 *ptr, ObjDelFn &fn)
{
	SPTR_DEBUG("++[CSharedBase<%s>]: CreateSharedBase from <%s>",
			   TYPE_NAME(T), TYPE_NAME(T1));

	if (nullptr == ptr) {
		throw ES("Null pointer is used to create CSharedBase");
	}

	DEFINE_POOL_BASE(Pool, sizeof(CSharedBase<T>));

	auto ret = new (Pool::Alloc()) CSharedBase<T>(ptr, fn, Pool::Release);

	SPTR_DEBUG("--[CSharedBase<%s>]: CreateSharedBase from <%s>",
			   TYPE_NAME(T), TYPE_NAME(T1));

	return ret;
}

template <class T>
static void DefObjDel(T *t)
{
	SPTR_DEBUG("++[CSharedBase<%s>]: delete object", TYPE_NAME(T));
	t->T::~T();
	SPTR_DEBUG("--[CSharedBase<%s>]: delete object", TYPE_NAME(T));
}

/* Both object and SharedBase are created by the SharedBase */
/* For lref initialization */
template <class T, class... Args>
static inline CSharedBase<T> *AllocCreateSharedBase(Args && ... args)
{
	SPTR_DEBUG("++[CSharedBase<%s>]: AllocCreateSharedBase. nParam: %lu",
			   TYPE_NAME(T), sizeof...(args));
	SPTR_VARIADIC_PRINT("\t<%s>\n", TYPE_NAME(decltype(args)));

	DEFINE_POOL_BASE(Pool, sizeof(CSharedBase<T>) + sizeof(T));

	char *buf = Pool::Alloc();
	T *ptr = new (buf + sizeof(CSharedBase<T>))
		T(std::forward<decltype(args)>(args)...);

	auto ret = new (buf) CSharedBase<T>(ptr, DefObjDel<T>, Pool::Release);

	SPTR_DEBUG("--[CSharedBase<%s>]: AllocCreateSharedBase. nParam: %lu",
			   TYPE_NAME(T), sizeof...(args));

	return ret;
}

#endif /* __SHARED_BASE_HPP__ */

