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

#ifndef __ENABLE_SHARED_PTR_HPP__
#define __ENABLE_SHARED_PTR_HPP__

#include "SharedBase.hpp"

template <class T>
class CSharedPtr;

template <class T>
class CWeakPtr;

HAS_MEMBER(ImEnableSharedPtr);
#define HAS_ENABLE_SHARED_PTR(T) \
	has_derived_member_ImEnableSharedPtr<T, CEnableSharedPtr<REMOVE_CONST(T)>>

template <class T>
class CEnableSharedPtr
{
private:
	CWeakPtr<T> mInst;
	mutable CWeakPtr<const T> mConstInst;

protected:
	inline CEnableSharedPtr(void);

public:
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(IS_CONST(T1)),
			 ENABLE_IF(MAYBE_ASSIGNABLE(const T, T1))>
	inline void SetShared(const CSharedPtr<T1> &ptr) const;

	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(!IS_CONST(T1)),
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
	inline void SetShared(const CSharedPtr<T1> &ptr);

	inline CSharedPtr<T> Share(void);
	inline CSharedPtr<const T> Share(void) const;

	/* For META program only */
	inline void ImEnableSharedPtr(void) const;

private:
	inline void Dump(void) const;
};

/* =====================================================================
 *							Implement CEnableSharedPtr
 * =====================================================================
 */
template <class T>
inline CEnableSharedPtr<T>::CEnableSharedPtr(void)
{
	SPTR_DEBUG(ENSPTR_HEAD() SPTR_PTR " default construct",
			   TYPE_NAME(T), this);
}

template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(IS_CONST(T1)),
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(const T, T1))>
inline void CEnableSharedPtr<T>::SetShared(const CSharedPtr<T1> &ptr) const
{
	SPTR_DEBUG_ENTRY(ENSPTR_HEAD() SPTR_PTR " SetShared const " SPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &ptr);

	Dump();
	mConstInst = ptr;
	Dump();

	SPTR_DEBUG_EXIT(ENSPTR_HEAD() SPTR_PTR " SetShared const " SPTR_HEAD() SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T1), &ptr);
}

template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(!IS_CONST(T1)),
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline void CEnableSharedPtr<T>::SetShared(const CSharedPtr<T1> &ptr)
{
	SPTR_DEBUG_ENTRY(ENSPTR_HEAD() SPTR_PTR " SetShared " SPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &ptr);

	Dump();
	mInst = ptr;
	mConstInst = ptr;
	Dump();

	SPTR_DEBUG_EXIT(ENSPTR_HEAD() SPTR_PTR " SetShared " SPTR_HEAD() SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T1), &ptr);
}

template <class T>
inline CSharedPtr<T> CEnableSharedPtr<T>::Share(void)
{
	SPTR_DEBUG_ENTRY(ENSPTR_HEAD() SPTR_PTR " share", TYPE_NAME(T), this);

	auto ret = mInst.Lock();

	SPTR_DEBUG_EXIT(ENSPTR_HEAD() SPTR_PTR " share", TYPE_NAME(T), this);

	return ret;
}

template <class T>
inline CSharedPtr<const T> CEnableSharedPtr<T>::Share(void) const
{
	SPTR_DEBUG_ENTRY(ENSPTR_HEAD() SPTR_PTR " share const", TYPE_NAME(T), this);

	auto ret = mConstInst.Lock();

	SPTR_DEBUG_EXIT(ENSPTR_HEAD() SPTR_PTR " share const", TYPE_NAME(T), this);

	return ret;
}

template <class T>
inline void CEnableSharedPtr<T>::Dump(void) const
{
	SPTR_DUMP("Addr: %p mInst: %p, mConstInst: %p, Type: %s",
			  this, &mInst, &mConstInst, TYPE_NAME(T));
}

#endif /* __ENABLE_SHARED_PTR_HPP__ */

