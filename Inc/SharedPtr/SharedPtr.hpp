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

#ifndef __SHARED_PTR_HPP__
#define __SHARED_PTR_HPP__

#include "SharedBase.hpp"
#include "WeakPtr.hpp"
#include "EnableSharedPtr.hpp"
#include "SharedToken.hpp"
#include "SharedDeleter.hpp"

/* Help function to create a CSharedPtr. */
template <class T, class... Args>
inline CSharedPtr<T> MakeShared(Args && ... args);

template <class T>
class CSharedPtr
{
private:
	/* Object */
	T *mPtr;

	/* Saves counter and deleter */
	CSharedBase<T> *mBase;

public: /* Constructor and Destructor */
	/* Construct for interface, an empty CSharedPtr is created. */
	template <class K = T,
			 ENABLE_IF(std::is_same<K, T>),
			 ENABLE_IF(std::is_abstract<K>)>
	inline CSharedPtr(void);

	/* Construct for class, a new T is created. */
	template <class K = T,
			 ENABLE_IF(std::is_same<K, T>),
			 ENABLE_IF(!std::is_abstract<K>)>
	inline CSharedPtr(void);

	/* Constructor from nullptr. */
	inline CSharedPtr(std::nullptr_t);

	/* Copy constructor [Must defined] */
	inline CSharedPtr(const CSharedPtr<T> &ptr);

	/* Move constructor [Must defined] */
	inline CSharedPtr(CSharedPtr<T> &&ptr);

	/* Constructor from CSharedPtr (lref) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
	inline CSharedPtr(const CSharedPtr<T1> &ptr);

	/* Constructor from CSharedPtr (rref) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
	inline CSharedPtr(CSharedPtr<T1> &&ptr);

	/* Constructor from pointer */
	template <class T1,
			 class Deleter = void(T *),
			 DEBUG_TEMPLATE,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
	inline CSharedPtr(T1 *ptr, const Deleter &deleter = DefaultDeleter);

	/* Constructor from pointer and CSharedBase. */
	inline explicit CSharedPtr(T *ptr, CSharedBase<T> *base);

	/* Constructor from multiple objects (overload) */
	template <class... Tn,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(!SPTR_CAN_SHARE(T, Tn && ...)),
			 ENABLE_IF(has_constructor<T, Tn && ...>)>
	inline CSharedPtr(Tn && ... tn);

	/* Destructor */
	inline ~CSharedPtr(void);

public:
	/* Adds reference and return this as a void *.
	 * The caller must manually convert it back to
	 * the CSharedPtr<T> with same type.
	 * Otherwise, there will be problems. */
	inline void *ToToken(void);

	/* Constructor from token */
	inline explicit CSharedPtr(const CSharedToken<T> *token);

public: /* operator = */
	/* operator = nullptr */
	inline CSharedPtr<T> &operator = (std::nullptr_t);

	/* Default copy */
	inline CSharedPtr<T> &operator = (const CSharedPtr<T> &t);

	/* operator = CSharedPtr (lref) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
	inline CSharedPtr<T> &operator = (const CSharedPtr<T1> &t);

	/* operator = CSharedPtr (rref) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
	inline CSharedPtr<T> &operator = (CSharedPtr<T1> &&t);

	/* operator = object (construct) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(SPTR_CAN_CONVERT(T, T1 &&))>
	inline CSharedPtr<T> &operator = (T1 &&t1);

	/* operator = object (overload) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(!SPTR_CAN_CONVERT(T, T1 &&)),
			 ENABLE_IF(has_member_operator_equal<T, T1 &&>)>
	inline CSharedPtr<T> &operator = (T1 &&t1);

public: /* operator == */
	/* CSharedPtr<T> == nullptr_t */
	inline bool operator == (std::nullptr_t) const;

	/* T == T1 (overloaded) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(has_member_operator_is_equal<T, T1 &&>)>
	inline bool operator == (T1 &&t1) const;

	/* T == T1 (construct) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(!has_member_operator_is_equal<T, T1 &&>),
			 ENABLE_IF(SPTR_CAN_CONVERT(T, T1 &&)),
			 class K = T,
			 ENABLE_IF(has_member_operator_is_equal<K, K &&>)>
	inline bool operator == (T1 &&t1) const;

public: /* operator > */
	/* T > T1 (overloaded) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(has_member_operator_bigger<T, T1 &&>)>
	inline bool operator > (T1 &&t1) const;

	/* T > T1 (construct) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(!has_member_operator_bigger<T, T1 &&>),
			 ENABLE_IF(SPTR_CAN_CONVERT(T, T1 &&)),
			 class K = T,
			 ENABLE_IF(has_member_operator_bigger<K, K &&>)>
	inline bool operator > (T1 &&t1) const;

public: /* operator < */
	/* T < T1 (overloaded) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(has_member_operator_smaller<T, T1 &&>)>
	inline bool operator < (T1 &&t1) const;

	/* T < T1 (construct) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(!has_member_operator_smaller<T, T1 &&>),
			 ENABLE_IF(SPTR_CAN_CONVERT(T, T1 &&)),
			 class K = T,
			 ENABLE_IF(has_member_operator_smaller<K, K &&>)>
	inline bool operator < (T1 &&t1) const;

public: /* operator += */
	/* CSharedPtr<T> += nullptr_t */
	inline void operator += (std::nullptr_t) const;

	/* T += T1 (overloaded) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(has_member_operator_add_equal<T, T1 &&>)>
	inline void operator += (T1 &&t1) const;

	/* T += T1 (construct) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(!has_member_operator_add_equal<T, T1 &&>),
			 ENABLE_IF(SPTR_CAN_CONVERT(T, T1 &&)),
			 class K = T,
			 ENABLE_IF(has_member_operator_add_equal<K, K &&>)>
	inline void operator += (T1 &&t1) const;

public: /* operator -= */
	/* CSharedPtr<T> -= nullptr_t */
	inline void operator -= (std::nullptr_t) const;

	/* T -= T1 (overloaded) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(has_member_operator_sub_equal<T, T1 &&>)>
	inline void operator -= (T1 &&t1) const;

	/* T -= T1 (construct) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(!has_member_operator_sub_equal<T, T1 &&>),
			 ENABLE_IF(SPTR_CAN_CONVERT(T, T1 &&)),
			 class K = T,
			 ENABLE_IF(has_member_operator_sub_equal<K, K &&>)>
	inline void operator -= (T1 &&t1) const;

public: /* Other operator */
	/* operator [] */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(has_member_operator_square_brackets<T, T1 &&>)>
	inline decltype(auto) operator [](T1 &&t1) const;

	/* bool is treated as int/uint32_t in C++
	 * Implement this will provide a way that
	 * CStringPtr<T> be implicitly converted
	 * into uint32_t equals to 1 or 0.
	 * This will cause some issue for construction
	 * Search "Safe Bool Idiom" for more information. */
private:
	typedef void (CSharedPtr<T>::* bool_t)() const;
	void bool_fn() const {}

public:
	/* operator bool */
	inline operator bool_t() const;

	/* operator function */
	template <class... Tn,
			 ENABLE_IF(has_operator_func<T, Tn &&...>)>
	inline decltype(auto) operator () (Tn &&... tn);

	template <class... Tn,
			 ENABLE_IF(has_operator_func<T, Tn &&...>)>
	inline decltype(auto) operator () (Tn &&... tn) const;

	/* operator -> */
	inline T *operator -> (void);
	inline T *operator -> (void) const;

	/* operator * */
	inline T &operator * (void) const;

public: /* Others */
	/* Release this shared pointer */
	inline void Release(void);

	/* Get the pointer from the shared pointer */
	inline T *Get(void) const;

	/* Swap. */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
	inline void Swap(CSharedPtr<T1> &t1);

	/* Get the reference information */
	inline uint32_t GetRef(void) const;
	inline uint32_t GetWeakRef(void) const;

public: /* Implicit convert */
	/* Implicit convert */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(!IS_CONST(T1)),
			 ENABLE_IF(SPTR_CAN_IMPLICIT_CONVERT(T1, T))>
	inline operator T1();

	/* Implicit convert(const) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(IS_CONST(T1)),
			 ENABLE_IF(SPTR_CAN_IMPLICIT_CONVERT(T1, T))>
	inline operator T1() const;

	/* Convert */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(!IS_CONST(T1)),
			 ENABLE_IF(SPTR_CAN_IMPLICIT_CONVERT(T1, T))>
	inline T1 Convert(void);

	/* Convert(const) */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(IS_CONST(T1)),
			 ENABLE_IF(SPTR_CAN_IMPLICIT_CONVERT(T1, T))>
	inline T1 Convert(void) const;

public: /* Deleter */
	/* default delete */
	static void DefaultDeleter(T *buf);

	/* delete [] */
	static void ArrayDeleter(T *buf);

	/* does nothing */
	static void NullDeleter(T *);

	/* For debug purpose only. */
	inline void Dump(void) const;

private: /* Help function */
	/* Replace the current base to the new one */
	template <class T1>
	inline void Replace(const CSharedPtr<T1> &ptr);

	template <class T1,
			 ENABLE_IF(std::is_same<T1, T>)>
	inline void CopyShared(void);

	template <class T1,
			 ENABLE_IF(!std::is_same<T1, T>)>
	inline void CopyShared(void);

	template <class K = T,
			 ENABLE_IF(HAS_ENABLE_SHARED_PTR(K))>
	inline void SetShared(void);

	template <class K = T,
			 ENABLE_IF(!HAS_ENABLE_SHARED_PTR(K))>
	inline void SetShared(void);

	/* Create SharedBase. */
	/* The user create the pointer and provide the deleter. */
	template <class T1,
			 class DeleterFn,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1)),
			 ENABLE_IF(IS_FUNC_POINTER(DeleterFn))>
	static inline CSharedPtr<T> CreateSharedBase(T1 *ptr, DeleterFn &fn);

	template <class T1,
			 class DeleterFn,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1)),
			 ENABLE_IF(!IS_FUNC_POINTER(DeleterFn))>
	static inline CSharedPtr<T> CreateSharedBase(T1 *ptr, DeleterFn &fn);

	/* Copy construct a SharedPtr from a valid input */
	template <class T1,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
	static inline CSharedPtr<T> CopySharedPtr(const CSharedPtr<T1> &t1);

public:
	/* Used only for meta programming. */
	inline void ImSharedPtr(void);

private: /* Friends */
	template <class T1>
	friend class CWeakPtr;

	template <class T1>
	friend class CSharedPtr;
};

/* =====================================================================
 *							Implement CSharedPtr
 * ===================================================================== */
/* Construct for interface, an empty CSharedPtr is created. */
template <class T>
template <class K,
		 DECLARE_ENABLE_IF(std::is_same<K, T>),
		 DECLARE_ENABLE_IF(std::is_abstract<K>)>
inline CSharedPtr<T>::CSharedPtr(void) :
	CSharedPtr(nullptr)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " default construct for interface",
			   TYPE_NAME(T), this);
}

/* Construct for class, a new T is created. */
template <class T>
template <class K,
		 DECLARE_ENABLE_IF(std::is_same<K, T>),
		 DECLARE_ENABLE_IF(!std::is_abstract<K>)>
inline CSharedPtr<T>::CSharedPtr(void) :
	CSharedPtr(MakeShared<T>())
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " default construct for class",
			   TYPE_NAME(T), this);
}

/* Constructor from nullptr. */
template <class T>
inline CSharedPtr<T>::CSharedPtr(std::nullptr_t) :
	mPtr(nullptr),
	mBase(nullptr)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " construct nullptr_t",
			   TYPE_NAME(T), this);
}

/* Copy constructor [Must defined] */
template <class T>
inline CSharedPtr<T>::CSharedPtr(const CSharedPtr<T> &ptr) :
	CSharedPtr(CopySharedPtr(ptr))
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " copy construct from "
			   SPTR_HEAD(" &") SPTR_PTR,
			   TYPE_NAME(T), this, TYPE_NAME(T), &ptr);
}

/* Move constructor [Must defined] */
template <class T>
inline CSharedPtr<T>::CSharedPtr(CSharedPtr<T> &&ptr) :
	mPtr(ptr.mPtr),
	mBase(ptr.mBase)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " move construct from "
					 SPTR_HEAD(" &&") SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T), &ptr);

	ptr.mPtr = nullptr;
	ptr.mBase = nullptr;
	Dump();

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " move construct from "
					SPTR_HEAD(" &&") SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T), &ptr);
}

/* Constructor from CSharedPtr (lref) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CSharedPtr<T>::CSharedPtr(const CSharedPtr<T1> &ptr) :
	CSharedPtr(CopySharedPtr(ptr))
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " construct from lref "
			   SPTR_HEAD(" &") SPTR_PTR,
			   TYPE_NAME(T), this, TYPE_NAME(T1), &ptr);
}

/* Constructor from CSharedPtr (rref) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CSharedPtr<T>::CSharedPtr(CSharedPtr<T1> &&ptr) :
	mPtr(DynamicCast<T1, T>(ptr.mPtr)),
	mBase((CSharedBase<T> *)ptr.mBase)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " construct from rref "
					 SPTR_HEAD(" &&") SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &ptr);

	ptr.mPtr = nullptr;
	ptr.mBase = nullptr;

	if (nullptr != mPtr) {
		CopyShared<T1>();
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " construct from rref "
					SPTR_HEAD(" &&") SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T1), &ptr);
}

/* Constructor from pointer */
template <class T>
template <class T1, class Deleter,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CSharedPtr<T>::CSharedPtr(T1 *ptr, const Deleter &deleter) :
	CSharedPtr(CreateSharedBase(ptr, deleter))
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " construct from "
			   SPTR_TYPE(" *") SPTR_PTR,
			   TYPE_NAME(T), this, TYPE_NAME(T1), ptr);
}

/* Constructor from pointer and CSharedBase. */
template <class T>
inline CSharedPtr<T>::CSharedPtr(T *ptr, CSharedBase<T> *base) :
	mPtr(ptr),
	mBase(base)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " construct from "
			   SBASE_HEAD(" *") SPTR_PTR " and " SPTR_TYPE(" *") SPTR_PTR,
			   TYPE_NAME(T), this, TYPE_NAME(T), base, TYPE_NAME(T), ptr);

	if (nullptr == mBase || nullptr == mPtr) {
		Dump();
		throw ES("Input CSharedBase or pointer cannot be nullptr");
	}

	SetShared();
}

/* Constructor from multiple objects (overload) */
template <class T>
template <class... Tn,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(!SPTR_CAN_SHARE(T, Tn && ...)),
		 DECLARE_ENABLE_IF(has_constructor<T, Tn && ...>)>
inline CSharedPtr<T>::CSharedPtr(Tn && ... tn) :
	CSharedPtr(MakeShared<T>(std::forward<decltype(tn)>(tn)...))
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " Construct from " SPTR_LONG " params",
			   TYPE_NAME(T), this, sizeof...(Tn));
}

/* Destructor */
template <class T>
inline CSharedPtr<T>::~CSharedPtr(void)
{
	SHARED_PTR_CHECK_DESTRUCTOR();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " destructor",
					 TYPE_NAME(T), this);

	Dump();
	Release();
	Dump();

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " destructor",
					TYPE_NAME(T), this);
}

/* Adds reference and return this as a void *.
 * The caller must manually convert it back to
 * the CSharedPtr<T> with same type.
 * Otherwise, there will be problems. */
template <class T>
inline void *CSharedPtr<T>::ToToken(void)
{
	SHARED_PTR_CHECK();
	return CSharedToken<T>::Create(*this);
}

/* Constructor from token */
template <class T>
inline CSharedPtr<T>::CSharedPtr(const CSharedToken<T> *token) :
	CSharedPtr(nullptr == token ? nullptr : token->mPtr)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " Construct from token: " SPTR_PTR,
			   TYPE_NAME(T), this, token);
}

/* operator = nullptr */
template <class T>
inline CSharedPtr<T> &CSharedPtr<T>::operator = (std::nullptr_t)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " =(N)",
					 TYPE_NAME(T), this);

	Release();

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " =(N)",
					TYPE_NAME(T), this);

	return *this;
}

/* Default copy */
template <class T>
inline CSharedPtr<T> &CSharedPtr<T>::operator = (const CSharedPtr<T> &t)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " =(&) " SPTR_HEAD(" &") SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T), &t);

	Replace(t);

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " =(&) " SPTR_HEAD(" &") SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T), &t);

	return *this;
}

/* operator = CSharedPtr (lref) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CSharedPtr<T> &CSharedPtr<T>::operator = (const CSharedPtr<T1> &t)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " =(&) " SPTR_HEAD(" &") SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t);

	Replace(t);

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " =(&) " SPTR_HEAD(" &") SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T1), &t);

	return *this;
}

/* operator = CSharedPtr (rref) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CSharedPtr<T> &CSharedPtr<T>::operator = (CSharedPtr<T1> &&t)
{
	SHARED_PTR_CHECK();
	Release();

	if (t) {
		SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " =(&&) " SPTR_HEAD(" &&") SPTR_PTR,
						 TYPE_NAME(T), this, TYPE_NAME(T1), &t);

		/* DynamicCast MUST do first.
		 * So when the exception happens,
		 * the mBase is still nullptr */
		mPtr = DynamicCast<T1, T>(t.mPtr);
		mBase = (CSharedBase<T> *)t.mBase;

		t.mPtr = nullptr;
		t.mBase = nullptr;

		CopyShared<T1>();

		SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " =(&&) " SPTR_HEAD(" &&") SPTR_PTR,
						TYPE_NAME(T), this, TYPE_NAME(T1), &t);
	} else {
		SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " =(&&) " SPTR_HEAD(" &&") SPTR_PTR " empty",
				   TYPE_NAME(T), this, TYPE_NAME(T1), &t);
	}

	return *this;
}

/* operator = object (construct) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(SPTR_CAN_CONVERT(T, T1 &&))>
inline CSharedPtr<T> &CSharedPtr<T>::operator = (T1 &&t1)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " =(c) " SPTR_TYPE() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	operator =(CSharedPtr<T>(std::forward<decltype(t1)>(t1)));

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " =(c) " SPTR_TYPE() SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	return *this;
}

/* operator = object (overload) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(!SPTR_CAN_CONVERT(T, T1 &&)),
		 DECLARE_ENABLE_IF(has_member_operator_equal<T, T1 &&>)>
inline CSharedPtr<T> &CSharedPtr<T>::operator = (T1 &&t1)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " =(o) " SPTR_TYPE() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	if (nullptr != mBase && nullptr != mPtr) {
		mPtr->operator =(std::forward<decltype(t1)>(t1));
	} else {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " =(o)", TYPE_NAME(T), this);
		Dump();
		throw ES("Cannot assign to a nullptr");
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " =(o) " SPTR_TYPE() SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	return *this;
}

/* CSharedPtr<T> == nullptr_t */
template <class T>
inline bool CSharedPtr<T>::operator == (std::nullptr_t) const
{
	bool ret;

	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " ==(N)",
					 TYPE_NAME(T), this);

	ret = (nullptr == mBase);

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " ==(N), result: %d",
					TYPE_NAME(T), this, ret);

	return ret;
}

/* T == T1 (overloaded) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(has_member_operator_is_equal<T, T1 &&>)>
inline bool CSharedPtr<T>::operator == (T1 &&t1) const
{
	bool ret;

	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " ==(&) " SPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	if (nullptr != mBase) {
		ret = (mPtr->operator == (std::forward<decltype(t1)>(t1)));
	} else {
		ret = false;
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " ==(&) "
					SPTR_HEAD() SPTR_PTR "result: %d",
					TYPE_NAME(T), this, TYPE_NAME(T1), &t1, ret);

	return ret;
}

/* T == T1 (construct) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(!has_member_operator_is_equal<T, T1 &&>),
		 DECLARE_ENABLE_IF(SPTR_CAN_CONVERT(T, T1 &&)),
		 class K,
		 DECLARE_ENABLE_IF(has_member_operator_is_equal<K, K &&>)>
inline bool CSharedPtr<T>::operator == (T1 &&t1) const
{
	bool ret;

	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " ==(c) " SPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	if (nullptr != mBase) {
		ret = (mPtr->operator == (
				*(CSharedPtr<T>(std::forward<decltype(t1)>(t1)))));
	} else {
		ret = false;
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " ==(c) "
					SPTR_HEAD() SPTR_PTR "result: %d",
					TYPE_NAME(T), this, TYPE_NAME(T1), &t1, ret);

	return ret;
}

/* T > T1 (overloaded) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(has_member_operator_bigger<T, T1 &&>)>
inline bool CSharedPtr<T>::operator > (T1 &&t1) const
{
	bool ret;

	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " >(o) " SPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	if (nullptr != mBase && nullptr != mPtr) {
		ret = (mPtr->operator > (std::forward<decltype(t1)>(t1)));
	} else {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " >(o)", TYPE_NAME(T), this);
		Dump();
		throw ES("Nullptr for operator >");
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " >(o) "
					SPTR_HEAD() SPTR_PTR "result: %d",
					TYPE_NAME(T), this, TYPE_NAME(T1), &t1, ret);

	return ret;
}

/* T > T1 (construct) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(!has_member_operator_bigger<T, T1 &&>),
		 DECLARE_ENABLE_IF(SPTR_CAN_CONVERT(T, T1 &&)),
		 class K,
		 DECLARE_ENABLE_IF(has_member_operator_bigger<K, K &&>)>
inline bool CSharedPtr<T>::operator > (T1 &&t1) const
{
	bool ret;

	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " >(c) " SPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	if (nullptr != mBase && nullptr != mPtr) {
		ret = (mPtr->operator > (
				*(CSharedPtr<T>(std::forward<decltype(t1)>(t1)))));
	} else {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " >(c)", TYPE_NAME(T), this);
		Dump();
		throw ES("Nullptr for operator >");
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " >(c) "
					SPTR_HEAD() SPTR_PTR "result: %d",
					TYPE_NAME(T), this, TYPE_NAME(T1), &t1, ret);

	return ret;
}

/* T < T1 (overloaded) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(has_member_operator_smaller<T, T1 &&>)>
inline bool CSharedPtr<T>::operator < (T1 &&t1) const
{
	bool ret;

	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " <(o) " SPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	if (nullptr != mBase && nullptr != mPtr) {
		ret = (mPtr->operator < (std::forward<decltype(t1)>(t1)));
	} else {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " <(o)", TYPE_NAME(T), this);
		Dump();
		throw ES("Nullptr for operator <");
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " <(o) "
					SPTR_HEAD() SPTR_PTR "result: %d",
					TYPE_NAME(T), this, TYPE_NAME(T1), &t1, ret);

	return ret;
}

/* T > T1 (construct) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(!has_member_operator_smaller<T, T1 &&>),
		 DECLARE_ENABLE_IF(SPTR_CAN_CONVERT(T, T1 &&)),
		 class K,
		 DECLARE_ENABLE_IF(has_member_operator_smaller<K, K &&>)>
inline bool CSharedPtr<T>::operator < (T1 &&t1) const
{
	bool ret;

	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " <(c) " SPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	if (nullptr != mBase && nullptr != mPtr) {
		ret = (mPtr->operator < (
				*(CSharedPtr<T>(std::forward<decltype(t1)>(t1)))));
	} else {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " <(c)", TYPE_NAME(T), this);
		Dump();
		throw ES("Nullptr for operator <");
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " <(c) "
					SPTR_HEAD() SPTR_PTR "result: %d",
					TYPE_NAME(T), this, TYPE_NAME(T1), &t1, ret);

	return ret;
}

/* CSharedPtr<T> += nullptr_t */
template <class T>
inline void CSharedPtr<T>::operator += (std::nullptr_t) const
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " +=(N)", TYPE_NAME(T), this);
}

/* T += T1 (overloaded) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(has_member_operator_add_equal<T, T1 &&>)>
inline void CSharedPtr<T>::operator += (T1 &&t1) const
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " +=(o) " SPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	if (nullptr != mBase && nullptr != mPtr) {
		(mPtr->operator += (std::forward<decltype(t1)>(t1)));
	} else {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " +=(o)", TYPE_NAME(T), this);
		Dump();
		throw ES("Nullptr for operator +=");
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " +=(o) " SPTR_HEAD() SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T1), &t1);
}

/* T += T1 (construct) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(!has_member_operator_add_equal<T, T1 &&>),
		 DECLARE_ENABLE_IF(SPTR_CAN_CONVERT(T, T1 &&)),
		 class K,
		 DECLARE_ENABLE_IF(has_member_operator_add_equal<K, K &&>)>
inline void CSharedPtr<T>::operator += (T1 &&t1) const
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " +=(c) " SPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	if (nullptr != mBase && nullptr != mPtr) {
		(mPtr->operator += (*(CSharedPtr<T>(std::forward<decltype(t1)>(t1)))));
	} else {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " +=(c)", TYPE_NAME(T), this);
		Dump();
		throw ES("Nullptr for operator +=");
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " +=(c) " SPTR_HEAD() SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T1), &t1);
}

/* CSharedPtr<T> -= nullptr_t */
template <class T>
inline void CSharedPtr<T>::operator -= (std::nullptr_t) const
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " -=(N)", TYPE_NAME(T), this);
}

/* T -= T1 (overloaded) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(has_member_operator_sub_equal<T, T1 &&>)>
inline void CSharedPtr<T>::operator -= (T1 &&t1) const
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " -=(o) " SPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	if (nullptr != mBase && nullptr != mPtr) {
		(mPtr->operator -= (std::forward<decltype(t1)>(t1)));
	} else {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " -=(o)", TYPE_NAME(T), this);
		Dump();
		throw ES("Nullptr for operator -=");
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " -=(o) " SPTR_HEAD() SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T1), &t1);
}

/* T -= T1 (construct) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(!has_member_operator_sub_equal<T, T1 &&>),
		 DECLARE_ENABLE_IF(SPTR_CAN_CONVERT(T, T1 &&)),
		 class K,
		 DECLARE_ENABLE_IF(has_member_operator_sub_equal<K, K &&>)>
inline void CSharedPtr<T>::operator -= (T1 &&t1) const
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " -=(c) " SPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), &t1);

	if (nullptr != mBase && nullptr != mPtr) {
		(mPtr->operator -= (*(CSharedPtr<T>(std::forward<decltype(t1)>(t1)))));
	} else {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " -=(c)", TYPE_NAME(T), this);
		Dump();
		throw ES("Nullptr for operator -=");
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " -=(c) " SPTR_HEAD() SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T1), &t1);
}

template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(has_member_operator_square_brackets<T, T1 &&>)>
inline decltype(auto) CSharedPtr<T>::operator [](T1 &&t1) const
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " []", TYPE_NAME(T), this);

	if (nullptr != mBase && nullptr != mPtr) {
		return (*mPtr)[std::forward<decltype(t1)>(t1)];
	} else {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " []", TYPE_NAME(T), this);
		Dump();
		throw ES("Illegal NULL[]");
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " []", TYPE_NAME(T), this);
}

template <class T>
inline CSharedPtr<T>::operator bool_t() const
{
	SHARED_PTR_CHECK();
	return (nullptr != mBase && nullptr != mPtr) ?
		&CSharedPtr<T>::bool_fn : 0;
}

template <class T>
template <class... Tn,
		 DECLARE_ENABLE_IF(has_operator_func<T, Tn && ...>)>
inline decltype(auto) CSharedPtr<T>::operator () (Tn && ... tn)
{
	SHARED_PTR_CHECK();
	if (nullptr == mBase || nullptr == mPtr) {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " ()", TYPE_NAME(T), this);
		Dump();
		throw ES("Empty CSharedPtr is used");
	}

	return (*(mPtr))(std::forward<decltype(tn)>(tn)...);
}

template <class T>
template <class... Tn,
		 DECLARE_ENABLE_IF(has_operator_func<T, Tn && ...>)>
inline decltype(auto) CSharedPtr<T>::operator () (Tn && ... tn) const
{
	SHARED_PTR_CHECK();
	if (nullptr == mBase || nullptr == mPtr) {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " ()(c)", TYPE_NAME(T), this);
		Dump();
		throw ES("Empty CSharedPtr is used");
	}

	return (*((const T*)(mPtr)))(std::forward<decltype(tn)>(tn)...);
}

template <class T>
inline T *CSharedPtr<T>::operator -> (void)
{
	SHARED_PTR_CHECK();
	if (nullptr == mBase || nullptr == mPtr) {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " ->", TYPE_NAME(T), this);
		Dump();
		throw ES("Empty CSharedPtr is used");
	}

	return mPtr;
}

template <class T>
inline T *CSharedPtr<T>::operator -> (void) const
{
	SHARED_PTR_CHECK();
	if (nullptr == mBase || nullptr == mPtr) {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " ->(c)", TYPE_NAME(T), this);
		Dump();
		throw ES("Empty CSharedPtr is used");
	}

	return mPtr;
}

template <class T>
inline T &CSharedPtr<T>::operator * (void) const
{
	SHARED_PTR_CHECK();
	if (nullptr == mBase || nullptr == mPtr) {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " *", TYPE_NAME(T), this);
		Dump();
		throw ES("Empty CSharedPtr is used");
	}

	return *mPtr;
}

template <class T>
inline void CSharedPtr<T>::Release(void)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " release", TYPE_NAME(T), this);

	if (nullptr != mBase) {
		mBase->ReleaseRef();
		mBase = nullptr;
		mPtr = nullptr;
	}

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " release", TYPE_NAME(T), this);
}

template <class T>
inline T *CSharedPtr<T>::Get(void) const
{
	SHARED_PTR_CHECK();
	if (nullptr == mBase || nullptr == mPtr) {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " Get", TYPE_NAME(T), this);
		Dump();
		throw ES("Empty CSharedPtr is used");
	}

	return mPtr;
}

template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline void CSharedPtr<T>::Swap(CSharedPtr<T1> &t1)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " swap to " SPTR_HEAD() SPTR_PTR,
					 TYPE_NAME(T), this, TYPE_NAME(T1), t1.Get());

	T1 *ptr = nullptr;
	CSharedBase<T1> *base = nullptr;

	/* t1 != nullptr */
	if (t1) {
		ptr = t1.mPtr;
		base = t1.mBase;
		t1.mBase = nullptr;
		t1.mPtr = nullptr;
	}

	if (nullptr != mBase) {
		t1.mPtr = DynamicCast<T, T1>(mPtr);
		t1.mBase = (CSharedBase<T1> *)mBase;
		t1.template CopyShared<T>();

		mBase = nullptr;
		mPtr = nullptr;
	}

	mPtr = DynamicCast<T1, T>(ptr);
	mBase = (CSharedBase<T> *)base;
	CopyShared<T1>();

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " swap to " SPTR_HEAD() SPTR_PTR,
					TYPE_NAME(T), this, TYPE_NAME(T1), t1.Get());
}

template <class T>
inline uint32_t CSharedPtr<T>::GetRef(void) const
{
	SHARED_PTR_CHECK();
	return (nullptr == mBase) ? 0 : mBase->mRef;
}

template <class T>
inline uint32_t CSharedPtr<T>::GetWeakRef(void) const
{
	SHARED_PTR_CHECK();
	return (nullptr == mBase) ? 0 : mBase->mWeakRef;
}

/* Convert */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(!IS_CONST(T1)),
		 DECLARE_ENABLE_IF(SPTR_CAN_IMPLICIT_CONVERT(T1, T))>
inline T1 CSharedPtr<T>::Convert(void)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " Convert to %s",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (nullptr == mBase || nullptr == mPtr) {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " Convert", TYPE_NAME(T), this);
		Dump();
		throw ES("Cannot convert from nullptr");
	}

	return mPtr->template Convert<T1>();
}

/* Convert(const) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(IS_CONST(T1)),
		 DECLARE_ENABLE_IF(SPTR_CAN_IMPLICIT_CONVERT(T1, T))>
inline T1 CSharedPtr<T>::Convert(void) const
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " Convert to const %s",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (nullptr == mBase || nullptr == mPtr) {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " Convert(c)", TYPE_NAME(T), this);
		Dump();
		throw ES("Cannot convert from nullptr");
	}

	return mPtr->template Convert<T1>();
}

/* Implicit convert */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(!IS_CONST(T1)),
		 DECLARE_ENABLE_IF(SPTR_CAN_IMPLICIT_CONVERT(T1, T))>
inline CSharedPtr<T>::operator T1()
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " Implicit convert to %s",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (nullptr == mBase || nullptr == mPtr) {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " implicit convert", TYPE_NAME(T), this);
		Dump();
		throw ES("Cannot implicit convert from nullptr");
	}

	return mPtr->template Convert<T1>();
}

/* Implicit convert(const) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(IS_CONST(T1)),
		 DECLARE_ENABLE_IF(SPTR_CAN_IMPLICIT_CONVERT(T1, T))>
inline CSharedPtr<T>::operator T1() const
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " Implicit convert to const %s",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (nullptr == mBase || nullptr == mPtr) {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " implicit convert(c)", TYPE_NAME(T), this);
		Dump();
		throw ES("Cannot implicit convert from nullptr");
	}

	return mPtr->template Convert<T1>();
}

template <class T>
inline void CSharedPtr<T>::Dump(void) const
{
	SHARED_PTR_CHECK();
	SPTR_DUMP("Addr: %p Base: %p Ptr: %p ref: %d wref: %d Type: %s",
			  this, mBase, mPtr, GetRef(), GetWeakRef(), TYPE_NAME(T));
}

template <class T>
void CSharedPtr<T>::DefaultDeleter(T *buf)
{
	SPTR_DEBUG(SPTR_HEAD() " DefaultDeleter", TYPE_NAME(T));

	delete buf;
}

template <class T>
void CSharedPtr<T>::ArrayDeleter(T *buf)
{
	SPTR_DEBUG(SPTR_HEAD() " ArrayDeleter", TYPE_NAME(T));

	delete [] buf;
}

template <class T>
void CSharedPtr<T>::NullDeleter(T *)
{
	SPTR_DEBUG(SPTR_HEAD() " NullDeleter", TYPE_NAME(T));
}

/* Replace the current base to the new one */
template <class T>
template <class T1>
inline void CSharedPtr<T>::Replace(const CSharedPtr<T1> &ptr)
{
	SHARED_PTR_CHECK();
	Release();

	if (ptr) {
		SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " <== replace == " SPTR_HEAD() SPTR_PTR,
						 TYPE_NAME(T), this, TYPE_NAME(T1), &ptr);

		mPtr = DynamicCast<T1, T>(ptr.mPtr);
		mBase = ptr.mBase->template AddRef<T>();
		CopyShared<T1>();

		SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " <== replace == " SPTR_HEAD() SPTR_PTR,
						TYPE_NAME(T), this, TYPE_NAME(T1), &ptr);
	} else {
		SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " <== replace == empty " SPTR_HEAD() SPTR_PTR,
				   TYPE_NAME(T), this, TYPE_NAME(T1), &ptr);
	}
}

template <class T>
template <class T1,
		 DECLARE_ENABLE_IF(std::is_same<T1, T>)>
inline void CSharedPtr<T>::CopyShared(void)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " CopyShared ignore", TYPE_NAME(T), this);
}

template <class T>
template <class T1,
		 DECLARE_ENABLE_IF(!std::is_same<T1, T>)>
inline void CSharedPtr<T>::CopyShared(void)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " CopyShared from " SPTR_HEAD(),
			   TYPE_NAME(T), this, TYPE_NAME(T1));
	SetShared();
}

template <class T>
template <class K,
		 DECLARE_ENABLE_IF(HAS_ENABLE_SHARED_PTR(K))>
inline void CSharedPtr<T>::SetShared(void)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG_ENTRY(SPTR_HEAD() SPTR_PTR " SetShared", TYPE_NAME(T), this);

	if (nullptr == mBase || nullptr == mPtr) {
		SPTR_ERROR(SPTR_HEAD() SPTR_PTR " SetShared", TYPE_NAME(T), this);
		Dump();
		throw ES("Set the shared base for empty CSharedPtr");
	}

	mPtr->CEnableSharedPtr<REMOVE_CONST(T)>::SetShared(*this);

	SPTR_DEBUG_EXIT(SPTR_HEAD() SPTR_PTR " SetShared", TYPE_NAME(T), this);
}

template <class T>
template <class K,
		 DECLARE_ENABLE_IF(!HAS_ENABLE_SHARED_PTR(K))>
inline void CSharedPtr<T>::SetShared(void)
{
	SHARED_PTR_CHECK();
	SPTR_DEBUG(SPTR_HEAD() SPTR_PTR " SetShared ignore", TYPE_NAME(T), this);
}

/* Used only for meta programming. */
template <class T>
inline void CSharedPtr<T>::ImSharedPtr(void)
{
	throw ES("ImSharedPtr should not be called");
}

/* Create SharedBase. */
/* The user create the pointer and provide the deleter. */
template <class T>
template <class T1,
		 class DeleterFn,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1)),
		 DECLARE_ENABLE_IF(IS_FUNC_POINTER(DeleterFn))>
inline CSharedPtr<T> CSharedPtr<T>::CreateSharedBase(T1 *ptr, DeleterFn &fn)
{

	if (nullptr != ptr) {
		SPTR_DEBUG_ENTRY(SPTR_HEAD() " CreateSharedBase ptr: " SPTR_PTR,
						 TYPE_NAME(T), ptr);

		typedef CSharedFuncPointerDeleter<T> CSharedDeleter;

		DEFINE_POOL_BASE(Pool, sizeof(CSharedBase<T>) + sizeof(CSharedDeleter), 
						 CSharedPtr<T>);
		char *buf = Pool::Alloc();

		try {
			T *_ptr = DynamicCast<T1, T>(ptr);

			char *tmp = buf + sizeof(CSharedBase<T>);
			CSharedDeleter *deleter = new (tmp) CSharedDeleter(_ptr, fn);

			auto base = new (buf) CSharedBase<T>(
					Pool::Release, (void *)deleter, CSharedDeleter::Delete);

			CSharedPtr<T> ret(_ptr, base);

			SPTR_DEBUG_EXIT(SPTR_HEAD() " CreateSharedBase ptr: " SPTR_PTR
							" base: " SPTR_PTR,
							TYPE_NAME(T), ptr, &ret);

			return ret;
		} catch (const IException *e) {
			Pool::Release(buf);
			SPTR_ERROR(SPTR_HEAD() " CreateSharedBase", TYPE_NAME(T));
			throw e;
		}

	} else {
		SPTR_DEBUG(SPTR_HEAD() " CreateSharedBase(N)",
				   TYPE_NAME(T));

		CSharedPtr<T> ret(nullptr);

		return ret;
	}
}

template <class T>
template <class T1,
		 class DeleterFn,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1)),
		 DECLARE_ENABLE_IF(!IS_FUNC_POINTER(DeleterFn))>
inline CSharedPtr<T> CSharedPtr<T>::CreateSharedBase(T1 *ptr, DeleterFn &fn)
{

	if (nullptr != ptr) {
		SPTR_DEBUG_ENTRY(SPTR_HEAD() " CreateSharedBase ptr: " SPTR_PTR,
						 TYPE_NAME(T), ptr);

		typedef CSharedLambdaDeleter<T, DeleterFn> CSharedDeleter;

		DEFINE_POOL_BASE(Pool, sizeof(CSharedBase<T>) + sizeof(CSharedDeleter),
						 CSharedPtr<T>);
		char *buf = Pool::Alloc();

		try {
			T *_ptr = DynamicCast<T1, T>(ptr);

			char *tmp = buf + sizeof(CSharedBase<T>);
			CSharedDeleter *deleter = new (tmp) CSharedDeleter(_ptr, fn);

			auto base = new (buf) CSharedBase<T>(
					Pool::Release, (void *)deleter, CSharedDeleter::Delete);

			CSharedPtr<T> ret(_ptr, base);

			SPTR_DEBUG_EXIT(SPTR_HEAD() " CreateSharedBase ptr: " SPTR_PTR
							" base: " SPTR_PTR,
							TYPE_NAME(T), ptr, &ret);

			return ret;
		} catch (const IException *e) {
			Pool::Release(buf);
			SPTR_ERROR(SPTR_HEAD() " CreateSharedBase", TYPE_NAME(T));
			throw e;
		}

	} else {
		SPTR_DEBUG(SPTR_HEAD() " CreateSharedBase(N)",
				   TYPE_NAME(T));

		CSharedPtr<T> ret(nullptr);

		return ret;
	}
}

/* Copy construct a SharedPtr from a valid input */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CSharedPtr<T> CSharedPtr<T>::CopySharedPtr(const CSharedPtr<T1> &t1)
{
	if (t1) {
		SPTR_DEBUG_ENTRY(SPTR_HEAD() " <== copy == " SPTR_HEAD() SPTR_PTR,
						 TYPE_NAME(T), TYPE_NAME(T1), &t1);
		t1.Dump();

		CSharedPtr<T> ret(DynamicCast<T1, T>(t1.mPtr),
						  t1.mBase->template AddRef<T>());

		ret.Dump();
		SPTR_DEBUG_EXIT(SPTR_HEAD() " <== copy == " SPTR_HEAD() SPTR_PTR,
						TYPE_NAME(T), TYPE_NAME(T1), &t1);

		return ret;

	} else {
		SPTR_DEBUG_ENTRY(SPTR_HEAD() " <== copy(N) == " SPTR_HEAD() SPTR_PTR,
						 TYPE_NAME(T), TYPE_NAME(T1), &t1);

		CSharedPtr<T> ret(nullptr);

		return ret;
	}
}

template <class T, class... Args>
inline CSharedPtr<T> MakeShared(Args && ... args)
{
	SPTR_DEBUG_ENTRY(SPTR_HEAD() " MakeShared. nParam: " SPTR_LONG,
					 TYPE_NAME(T), sizeof...(args));
	SPTR_VARIADIC_PRINT("\t" SPTR_TYPE() "\n", TYPE_NAME(decltype(args)));

	typedef CSharedDefaultDeleter<T> CSharedDeleter;

	DEFINE_POOL_BASE(Pool, sizeof(CSharedBase<T>) + sizeof(T) + sizeof(CSharedDeleter),
					 CSharedPtr<T>);
	char *buf = Pool::Alloc();

	if (nullptr == buf) {
		SPTR_ERROR(SPTR_HEAD() " MakeShared", TYPE_NAME(T));
		throw ES("MakeShared failed. Out of memory");
	}

	try {
		char *tmp = buf + sizeof(CSharedBase<T>);
		T *ptr = new (tmp) T(std::forward<decltype(args)>(args)...);

		tmp += sizeof(T);
		CSharedDeleter *deleter = new (tmp) CSharedDeleter(ptr);

		CSharedBase<T> *base = new (buf) CSharedBase<T>(
				Pool::Release, (void *)deleter, CSharedDeleter::Delete);

		CSharedPtr<T> ret(ptr, base);

		SPTR_DEBUG_EXIT(SPTR_HEAD() " MakeShared. nParam: " SPTR_LONG,
						TYPE_NAME(T), sizeof...(args));
		return ret;

	} catch (const IException *e) {
		Pool::Release(buf);
		SPTR_ERROR(SPTR_HEAD() " MakeShared", TYPE_NAME(T));
		throw e;
	}
}

#include "SharedPtrOverload.hpp"
#include "SharedToken.hpp"

#endif /* __SHARED_PTR_HPP__ */

