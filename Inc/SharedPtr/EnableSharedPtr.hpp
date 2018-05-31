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
	inline void SetSharedBase(CSharedBase<T1> &base) const;

	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(!IS_CONST(T1)),
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
	inline void SetSharedBase(CSharedBase<T1> &base);

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
	SPTR_DEBUG("[CEnableSharedPtr<%s>]: construct default", TYPE_NAME(T));
	Dump();
}

template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(IS_CONST(T1)),
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(const T, T1))>
inline void CEnableSharedPtr<T>::SetSharedBase(CSharedBase<T1> &base) const
{
	SPTR_DEBUG("++[CEnableSharedPtr<%s>]: SetSharedBase from CSharedBase<%s>",
			   TYPE_NAME(T), TYPE_NAME(T1));

	Dump();
	mConstInst = base;
	Dump();

	SPTR_DEBUG("--[CEnableSharedPtr<%s>]: SetSharedBase from CSharedBase<%s>",
			   TYPE_NAME(T), TYPE_NAME(T1));
}

template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(!IS_CONST(T1)),
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline void CEnableSharedPtr<T>::SetSharedBase(CSharedBase<T1> &base)
{
	SPTR_DEBUG("++[CEnableSharedPtr<%s>]: SetSharedBase from CSharedBase<%s>",
			   TYPE_NAME(T), TYPE_NAME(T1));

	Dump();
	mInst = base;
	mConstInst = base;
	Dump();

	SPTR_DEBUG("--[CEnableSharedPtr<%s>]: SetSharedBase from CSharedBase<%s>",
			   TYPE_NAME(T), TYPE_NAME(T1));
}

template <class T>
inline CSharedPtr<T> CEnableSharedPtr<T>::Share(void)
{
	SPTR_DEBUG("++[CEnableSharedPtr<%s>]: share", TYPE_NAME(T));

	Dump();
	auto ret = mInst.Lock();
	Dump();

	SPTR_DEBUG("--[CEnableSharedPtr<%s>]: share", TYPE_NAME(T));

	return ret;
}

template <class T>
inline CSharedPtr<const T> CEnableSharedPtr<T>::Share(void) const
{
	SPTR_DEBUG("++[CEnableSharedPtr<%s>]: share const", TYPE_NAME(T));

	Dump();
	auto ret = mConstInst.Lock();
	Dump();

	SPTR_DEBUG("--CEnableSharedPtr<%s>]: share const", TYPE_NAME(T));

	return ret;
}

template <class T>
inline void CEnableSharedPtr<T>::Dump(void) const
{
	SPTR_DEBUG("[CEnableSharedPtr<%s>(%p)]: mInst: %p, mConstInst: %p",
			   TYPE_NAME(T), this, &mInst, &mConstInst);
}

#endif /* __ENABLE_SHARED_PTR_HPP__ */

