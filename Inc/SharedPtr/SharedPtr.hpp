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

#include <typeinfo>
#include <Pool/PoolBase.hpp>
#include <ExceptionSimple.hpp>

#include "SharedBase.hpp"
#include "WeakPtr.hpp"
#include "EnableSharedPtr.hpp"
#include "SharedMeta.hpp"

template <class T>
class CSharedPtr
{
private:
	CSharedBase<T> *mBase;

public: /* Constructor and Destructor */
	/* Construct for interface, an empty CSharedPtr is created. */
	template <class K = T,
			 ENABLE_IF(std::is_abstract<K>)>
	inline CSharedPtr(void);

	/* Construct for class, a new T is created. */
	template <class K = T,
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
	template <class TPtr,
			 class Deleter = void(T *),
			 DEBUG_TEMPLATE,
			 ENABLE_IF(MAYBE_ASSIGNABLE(T, TPtr))>
	inline CSharedPtr(TPtr *ptr, const Deleter &deleter = DefaultDeleter);

	/* Constructor from CSharedBase (same type) */
	inline CSharedPtr(CSharedBase<T> *ptr);

	/* Constructor from multiple objects (overload) */
	template <class... Tn,
			 DEBUG_TEMPLATE,
			 ENABLE_IF(!SPTR_CAN_SHARE(T, Tn && ...)),
			 ENABLE_IF(has_constructor<T, Tn && ...>)>
	inline CSharedPtr(Tn && ... tn);

	/* Destructor */
	inline ~CSharedPtr(void);

public: /* Token */
	/* Token is a pointer that can be used by C API
	 * as the pdata of a callback.
	 * When the CSharedPtr<T> is converted into a token,
	 * its reference will be added by 1.
	 * In the callback function, the token can be
	 * converted back to the CSharedPtr<T> by:
	 * CSharedPtr<T> ptr((CSharedPtr<T>::Token *)token);
	 */
	class Token {
	public:
		inline void Release(void);

	private:
		/* A pure virtual function is used to prevent
		 * an instance of the Token is created. */
		virtual void NoInst(void) = 0;
	};

	/* Constructor from token.
	 * If bCopy == false(default), the reference will NOT be added.
	 *   so the token CANNOT be used to construct multiple times.
	 * If bCopy == true, The reference will be added.
	 *   so the token can be used to construct multiple times.
	 */
	inline CSharedPtr(Token *token, bool bCopy = false);

	inline Token *ToToken(void);

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

public: /* Other operator */
	/* operator += object (overload) */
	template <class T1,
			 ENABLE_IF(has_member_operator_add_equal<T, const T1 &>)>
	inline void operator += (const T1 &t1);

	/* operator += CSharedPtr (overload) */
	template <class T1,
			 ENABLE_IF(has_member_operator_add_equal<T, const T1 &>)>
	inline void operator += (const CSharedPtr<T1> &t1);

	/* operator [] */
	template <class T1>
	inline decltype(auto) operator [](T1 p) const;

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
			 ENABLE_IF(has_operator_func<T, const Tn &...>)>
	inline decltype(auto) operator () (const Tn &... tn);

	template <class... Tn,
			 ENABLE_IF(has_operator_func<T, const Tn &...>)>
	inline decltype(auto) operator () (const Tn &... tn) const;

	/* operator -> */
	inline T *operator -> (void);
	inline T *operator -> (void) const;

	/* operator * */
	inline T &operator * (void) const;

	/* operator > */
	template <class T1,
			 ENABLE_IF(has_member_operator_bigger<T, const T1 &>)>
	inline bool operator > (const CSharedPtr<T1> &t1) const;

	/* operator < */
	template <class T1,
			 ENABLE_IF(has_member_operator_smaller<T, const T1 &>)>
	inline bool operator < (const CSharedPtr<T1> &t1) const;

public: /* Others */
	/* Release this shared pointer */
	inline void Release(void);

	/* Get the pointer from the shared pointer */
	inline T *Get(void) const;

	/* Swap from same type. */
	inline void Swap(CSharedPtr<T> &p);

	/* Swap from child. */
	template <class T1,
			 ENABLE_IF(std::is_base_of<T, T1>),
			 ENABLE_IF(!std::is_base_of<T1, T>)>
	inline void Swap(CSharedPtr<T1> &p);

	/* Swap from parent. */
	template <class T1,
			 ENABLE_IF(!std::is_base_of<T, T1>),
			 ENABLE_IF(std::is_base_of<T1, T>)>
	inline void Swap(CSharedPtr<T1> &p);

	/* Get the reference information */
	inline uint32_t GetRef(void) const;
	inline uint32_t GetWeakRef(void) const;

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

	inline void Dump(void) const;

public: /* Deleter */
	/* delete */
	static void DefaultDeleter(T *buf);

	/* delete [] */
	static void ArrayDeleter(T *buf);

	/* does nothing */
	static void NullDeleter(T *);

private: /* Help function */
	/* Replace the current base to the new one */
	inline void Replace(CSharedBase<T> *base);

	template <class K = T,
			 ENABLE_IF(has_member_ImEnableSharedPtr<K>)>
	inline void SetShared(void);

	template <class K = T,
			 ENABLE_IF(!has_member_ImEnableSharedPtr<K>)>
	inline void SetShared(void);

public:
	/* Used only for meta programming. */
	inline void ImSharedPtr(void);

private: /* Friends */
	friend class CWeakPtr<T>;

	template <class T1>
	friend class CSharedPtr;
};

/* =====================================================================
 *							Implement CSharedPtr
 * ===================================================================== */
/* Construct for interface, an empty CSharedPtr is created. */
template <class T>
template <class K,
		 DECLARE_ENABLE_IF(std::is_abstract<K>)>
inline CSharedPtr<T>::CSharedPtr(void) :
	mBase(nullptr)
{
	SPTR_DEBUG("[CSharedPtr<%s>(%p)]: construct default for interface",
			   TYPE_NAME(T), this);
}

/* Construct for class, a new T is created. */
template <class T>
template <class K,
		 DECLARE_ENABLE_IF(!std::is_abstract<K>)>
inline CSharedPtr<T>::CSharedPtr(void) :
#ifdef DEBUG_SPTR
	mBase(nullptr)
#else
	mBase(AllocCreateSharedBase<T>())
#endif
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: construct for class",
			   TYPE_NAME(T), this);

#ifdef DEBUG_SPTR
	mBase = AllocCreateSharedBase<T>();
#endif
	SetShared();

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: construct for class, result: %p",
			   TYPE_NAME(T), this, mBase);
}

/* Constructor from nullptr. */
template <class T>
inline CSharedPtr<T>::CSharedPtr(std::nullptr_t) :
	mBase(nullptr)
{
	SPTR_DEBUG("[CSharedPtr<%s>(%p)]: construct nullptr_t",
			   TYPE_NAME(T), this);
}

/* Copy constructor [Must defined] */
template <class T>
inline CSharedPtr<T>::CSharedPtr(const CSharedPtr<T> &ptr) :
	mBase(nullptr)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: copy construct",
			   TYPE_NAME(T), this);

	if (ptr) {
		ptr.Dump();
		mBase = ptr.mBase->template AddRef<T>();
		SetShared();
		ptr.Dump();
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: copy construct, result: %p",
			   TYPE_NAME(T), this, mBase);
}

/* Move constructor [Must defined] */
template <class T>
inline CSharedPtr<T>::CSharedPtr(CSharedPtr<T> &&ptr) :
	mBase(nullptr)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: move construct",
			   TYPE_NAME(T), this);

	if (ptr) {
		ptr.Dump();
		mBase = ptr.mBase;
		ptr.mBase = nullptr;
		SetShared();
		Dump();
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: move construct, result: %p",
			   TYPE_NAME(T), this, mBase);
}

/* Constructor from CSharedPtr (lref) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CSharedPtr<T>::CSharedPtr(const CSharedPtr<T1> &ptr) :
	mBase(nullptr)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: construct from lref (CSharedPtr<%s> &)",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (ptr) {
		ptr.Dump();
		DynamicCast<T1, T>(ptr.Get());
		mBase = ptr.mBase->template AddRef<T>();
		SetShared();
		ptr.Dump();
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: construct from lref (CSharedPtr<%s> &), result: %p",
			   TYPE_NAME(T), this, TYPE_NAME(T1), mBase);
}

/* Constructor from CSharedPtr (rref) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CSharedPtr<T>::CSharedPtr(CSharedPtr<T1> &&ptr) :
	mBase(nullptr)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: construct from rref (CSharedPtr<%s> &&)",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (ptr) {
		ptr.Dump();
		DynamicCast<T1, T>(ptr.Get());
		mBase = (CSharedBase<T> *)ptr.mBase;
		ptr.mBase = nullptr;
		SetShared();
		Dump();
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: construct from rref (CSharedPtr<%s> &&), result: %p",
			   TYPE_NAME(T), this, TYPE_NAME(T1), mBase);
}

/* Constructor from pointer */
template <class T>
template <class TPtr, class Deleter,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, TPtr))>
inline CSharedPtr<T>::CSharedPtr(TPtr *ptr, const Deleter &deleter) :
#ifdef DEBUG_SPTR
	mBase(nullptr)
#else
	mBase(CreateSharedBase<T>(ptr, deleter))
#endif
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: construct from [%s *(%p)]",
			   TYPE_NAME(T), this, TYPE_NAME(TPtr), ptr);

#ifdef DEBUG_SPTR
	mBase = CreateSharedBase<T>(ptr, deleter);
	Dump();
#endif

	SetShared();

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: construct from [%s *(%p)], result: %p",
			   TYPE_NAME(T), this, TYPE_NAME(TPtr), ptr, mBase);
}

/* Constructor from CSharedBase (same type) */
template <class T>
inline CSharedPtr<T>::CSharedPtr(CSharedBase<T> *ptr) :
	mBase(ptr)
{
	SPTR_DEBUG("  [CSharedPtr<%s>(%p)]: construct from [CSharedBase<%s> &(%p)]",
			   TYPE_NAME(T), this, TYPE_NAME(T), ptr);

	if (nullptr == ptr) {
		throw ES("Input CSharedBase cannot be nullptr");
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
#ifdef DEBUG_SPTR
	mBase(nullptr)
#else
	mBase(AllocCreateSharedBase<T>(
			std::forward<decltype(tn)>(tn)...))
#endif
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: Construct from %lu const overload:",
			   TYPE_NAME(T), this, sizeof...(Tn));
	SPTR_VARIADIC_PRINT("\t<%s>\n",  TYPE_NAME(Tn));

#ifdef DEBUG_SPTR
	mBase = AllocCreateSharedBase<T>(std::forward<decltype(tn)>(tn)...);
#endif

	SetShared();

	SPTR_PRINT("--[CSharedPtr<%s>(%p)]: Construct from %lu const overload.",
			   TYPE_NAME(T), this, sizeof...(Tn));
}

/* Destructor */
template <class T>
inline CSharedPtr<T>::~CSharedPtr(void)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: destructor %p",
			   TYPE_NAME(T), this, mBase);

	Release();

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: destructor %p",
			   TYPE_NAME(T), this, mBase);
}

template <class T>
inline CSharedPtr<T>::CSharedPtr(Token *token, bool bCopy) :
	mBase(nullptr)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: %s construct from (Token *)%p",
			   TYPE_NAME(T), this, bCopy ? "copy" : "move", token);

	if (nullptr == token) {
		throw ES("A NULL token is used to construct");
	}

	mBase = (CSharedBase<T> *)token;

	if (bCopy) {
		mBase->AddRef();
	}

	SetShared();

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: %s construct from (Token *)%p",
			   TYPE_NAME(T), this, bCopy ? "copy" : "move", token);
}

template <class T>
inline void CSharedPtr<T>::Token::Release(void)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: token release", TYPE_NAME(T), this);

	((CSharedBase<T> *)(this))->ReleaseRef();

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: token release", TYPE_NAME(T), this);
}

template <class T>
inline typename CSharedPtr<T>::Token *CSharedPtr<T>::ToToken(void)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: to token", TYPE_NAME(T), this);

	mBase->AddRef();

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: to token", TYPE_NAME(T), this);

	return (Token *)mBase;
}

/* operator = nullptr */
template <class T>
inline CSharedPtr<T> &CSharedPtr<T>::operator = (std::nullptr_t)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: = nullptr", TYPE_NAME(T), this);
	Release();
	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: = nullptr", TYPE_NAME(T), this);
	return *this;
}

/* Default copy */
template <class T>
inline CSharedPtr<T> &CSharedPtr<T>::operator = (const CSharedPtr<T> &t)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: copy = CSharedPtr<%s>",
			   TYPE_NAME(T), this, TYPE_NAME(T));

	if (t) {
		Replace(t.mBase->template AddRef<T>());
	} else {
		Release();
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: copy = CSharedPtr<%s>",
			   TYPE_NAME(T), this, TYPE_NAME(T));

	return *this;
}

/* operator = CSharedPtr (lref) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CSharedPtr<T> &CSharedPtr<T>::operator = (const CSharedPtr<T1> &t)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: = CSharedPtr<%s> &",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (t) {
		DynamicCast<T1, T>(t.Get());
		Replace(t.mBase->template AddRef<T>());
	} else {
		Release();
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: = CSharedPtr<%s> &",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	return *this;
}

/* operator = CSharedPtr (rref) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(MAYBE_ASSIGNABLE(T, T1))>
inline CSharedPtr<T> &CSharedPtr<T>::operator = (CSharedPtr<T1> &&t)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)] = CSharedPtr<%s> &&",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (t) {
		DynamicCast<T1, T>(t.Get());
		Replace((CSharedBase<T> *)t.mBase);
		t.mBase = nullptr;
	} else {
		Release();
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)] = CSharedPtr<%s> &&",
			   TYPE_NAME(T), this, TYPE_NAME(T1));
	return *this;
}

/* operator = object (construct) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(SPTR_CAN_CONVERT(T, T1 &&))>
inline CSharedPtr<T> &CSharedPtr<T>::operator = (T1 &&t1)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: = %s (overload)",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	operator =(CSharedPtr<T>(std::forward<decltype(t1)>(t1)));

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: = %s (overload)",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

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
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: = %s (overload)",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (nullptr != mBase) {
		mBase->mPtr->operator =(std::forward<decltype(t1)>(t1));
	} else {
		throw ES("Cannot assign to a nullptr");
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: = %s (overload)",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	return *this;
}

/* CSharedPtr<T> == nullptr_t */
template <class T>
inline bool CSharedPtr<T>::operator == (std::nullptr_t) const
{
	bool ret = (nullptr == mBase);

	SPTR_DEBUG("[CSharedPtr<%s>(%p)] == [nullptr_t], result: %d",
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

	if (nullptr != mBase) {
		ret = (mBase->mPtr->operator == (std::forward<decltype(t1)>(t1)));
	} else {
		ret = false;
	}

	SPTR_DEBUG("[CSharedPtr<%s>(%p)] == [%s(%p)], result: %d",
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

	if (nullptr != mBase) {
		ret = (mBase->mPtr->operator == (
				CSharedPtr<T>(std::forward<decltype(t1)>(t1))));
	} else {
		ret = false;
	}

	SPTR_DEBUG("[CSharedPtr<%s>(%p)] == [%s(%p)], result: %d",
			   TYPE_NAME(T), this, TYPE_NAME(T1), &t1, ret);

	return ret;
}

template <class T>
template <class T1,
		 DECLARE_ENABLE_IF(has_member_operator_add_equal<T, const T1 &>)>
inline void CSharedPtr<T>::operator += (const T1 &t1)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: += CSharedPtr<>(%s)",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (nullptr != mBase) {
		(*(mBase->mPtr)) += t1;
	} else {
		throw ES("Illegal NULL += xxx");
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: += CSharedPtr<>(%s)",
			   TYPE_NAME(T), this, TYPE_NAME(T1));
}

template <class T>
template <class T1,
		 DECLARE_ENABLE_IF(has_member_operator_add_equal<T, const T1 &>)>
inline void CSharedPtr<T>::operator += (const CSharedPtr<T1> &t1)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: += CSharedPtr<%s>",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (nullptr == mBase) {
		throw ES("Illegal NULL += xxx");
	} else {
		if (t1) {
			(*(mBase->mPtr)) += (*t1);
		}
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: += CSharedPtr<%s>",
			   TYPE_NAME(T), this, TYPE_NAME(T1));
}

template <class T>
template <class T1>
inline decltype(auto) CSharedPtr<T>::operator [](T1 p) const
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: []", TYPE_NAME(T), this);

	if (nullptr != mBase) {
		return (*(mBase->mPtr))[p];
	} else {
		throw ES("Illegal NULL[]");
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: []", TYPE_NAME(T), this);
}

template <class T>
inline CSharedPtr<T>::operator bool_t() const
{
	return nullptr != mBase ?  &CSharedPtr<T>::bool_fn : 0;
}

template <class T>
template <class... Tn,
		 DECLARE_ENABLE_IF(has_operator_func<T, const Tn &...>)>
inline decltype(auto) CSharedPtr<T>::operator () (const Tn &... tn)
{
	if (nullptr == mBase) {
		throw ES("Empty CSharedPtr is used");
	}

	return (*(mBase->mPtr))(tn...);
}

template <class T>
template <class... Tn,
		 DECLARE_ENABLE_IF(has_operator_func<T, const Tn &...>)>
inline decltype(auto) CSharedPtr<T>::operator () (const Tn &... tn) const
{
	if (nullptr == mBase) {
		throw ES("Empty CSharedPtr is used");
	}

	return (*((const T*)(mBase->mPtr)))(tn...);
}

template <class T>
inline T *CSharedPtr<T>::operator -> (void)
{
	if (nullptr == mBase) {
		throw ES("Empty CSharedPtr is used");
	}

	return mBase->mPtr;
}

template <class T>
inline T *CSharedPtr<T>::operator -> (void) const
{
	if (nullptr == mBase) {
		throw ES("Empty CSharedPtr is used");
	}

	return mBase->mPtr;
}

template <class T>
inline T &CSharedPtr<T>::operator * (void) const
{
	if (nullptr == mBase) {
		throw ES("Empty CSharedPtr is used");
	}

	return *(mBase->mPtr);
}

template <class T>
template <class T1,
		 DECLARE_ENABLE_IF(has_member_operator_bigger<T, const T1 &>)>
inline bool CSharedPtr<T>::operator > (const CSharedPtr<T1> &t1) const
{
	if (nullptr == mBase) {
		/* null vs any */
		return false;
	} else {
		return t1 ?
			/* not-null vs not-null */
			mBase->mPtr->operator > (*t1) :
			/* not-null vs null */
			true;
	}
}

template <class T>
template <class T1,
		 DECLARE_ENABLE_IF(has_member_operator_smaller<T, const T1 &>)>
inline bool CSharedPtr<T>::operator < (const CSharedPtr<T1> &t1) const
{
	if (nullptr == mBase) {
		return t1 ?
			/* null vs not-null */
			true :
			/* null vs null */
			false;
	} else {
		return t1 ?
			/* not-null vs not-null */
			mBase->mPtr->operator < (*t1) :
			/* not-null vs null */
			false;
	}
}

template <class T>
inline void CSharedPtr<T>::Release(void)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: release", TYPE_NAME(T), this);

	if (nullptr != mBase) {
		mBase->ReleaseRef();
		mBase = nullptr;
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: release", TYPE_NAME(T), this);
}

template <class T>
inline T *CSharedPtr<T>::Get(void) const
{
	if (nullptr == mBase) {
		throw ES("Empty CSharedPtr is used");
	}

	return mBase->mPtr;
}

template <class T>
inline void CSharedPtr<T>::Swap(CSharedPtr<T> &p)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: swap to CSharedPtr<%s>",
			   TYPE_NAME(T), this, TYPE_NAME(T));

	auto tmp = p.mBase;
	p.mBase = mBase;
	mBase = tmp;

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: swap to CSharedPtr<%s>",
			   TYPE_NAME(T), this, TYPE_NAME(T));
}

template <class T>
template <class T1,
		 DECLARE_ENABLE_IF(std::is_base_of<T, T1>),
		 DECLARE_ENABLE_IF(!std::is_base_of<T1, T>)>
inline void CSharedPtr<T>::Swap(CSharedPtr<T1> &p)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: swap to child CSharedPtr<%s>",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (nullptr != mBase) {
		/* Make sure I can be casted to my child */
		DynamicCast<T, T1>(mBase->mPtr);
	}

	auto tmp = p.mBase;
	p.mBase = (CSharedBase<T1> *)mBase;
	mBase = (CSharedBase<T> *)tmp;

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: swap to child CSharedPtr<%s>",
			   TYPE_NAME(T), this, TYPE_NAME(T1));
}

template <class T>
template <class T1,
		 DECLARE_ENABLE_IF(!std::is_base_of<T, T1>),
		 DECLARE_ENABLE_IF(std::is_base_of<T1, T>)>
inline void CSharedPtr<T>::Swap(CSharedPtr<T1> &p)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: swap to base CSharedPtr<%s>",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (nullptr != mBase) {
		if (p) {
			/* Make sure my parent can be casted to me */
			DynamicCast<T1, T>(mBase->mPtr);
		}

		auto tmp = p.mBase;
		p.mBase = (CSharedBase<T1> *)mBase;
		mBase = (CSharedBase<T> *)tmp;
	} else {
		mBase = (CSharedBase<T> *)p.mBase;
		p.mBase = nullptr;
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: swap to base CSharedPtr<%s>",
			   TYPE_NAME(T), this, TYPE_NAME(T1));
}

template <class T>
inline uint32_t CSharedPtr<T>::GetRef(void) const
{
	return (nullptr == mBase) ? 0 : mBase->mRef;
}

template <class T>
inline uint32_t CSharedPtr<T>::GetWeakRef(void) const
{
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
	SPTR_DEBUG("[CSharedPtr<%s>(%p)]: Convert to %s",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (nullptr == mBase) {
		throw ES("Cannot convert from nullptr");
	}

	return mBase->mPtr->template Convert<T1>();
}

/* Convert(const) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(IS_CONST(T1)),
		 DECLARE_ENABLE_IF(SPTR_CAN_IMPLICIT_CONVERT(T1, T))>
inline T1 CSharedPtr<T>::Convert(void) const
{
	SPTR_DEBUG("[CSharedPtr<%s>(%p)]: Convert to const %s",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (nullptr == mBase) {
		throw ES("Cannot convert from nullptr");
	}

	return mBase->mPtr->template Convert<T1>();
}

/* Implicit convert */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(!IS_CONST(T1)),
		 DECLARE_ENABLE_IF(SPTR_CAN_IMPLICIT_CONVERT(T1, T))>
inline CSharedPtr<T>::operator T1()
{
	SPTR_DEBUG("[CSharedPtr<%s>(%p)]: Implicit convert to %s",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (nullptr == mBase) {
		throw ES("Cannot implicit convert from nullptr");
	}

	return mBase->mPtr->template Convert<T1>();
}

/* Implicit convert(const) */
template <class T>
template <class T1,
		 DECLARE_DEBUG_TEMPLATE,
		 DECLARE_ENABLE_IF(IS_CONST(T1)),
		 DECLARE_ENABLE_IF(SPTR_CAN_IMPLICIT_CONVERT(T1, T))>
inline CSharedPtr<T>::operator T1() const
{
	SPTR_DEBUG("[CSharedPtr<%s>(%p)]: Implicit convert to const %s",
			   TYPE_NAME(T), this, TYPE_NAME(T1));

	if (nullptr == mBase) {
		throw ES("Cannot implicit convert from nullptr");
	}

	return mBase->mPtr->template Convert<T1>();
}

template <class T>
inline void CSharedPtr<T>::Dump(void) const
{
	SPTR_DEBUG("[CSharedPtr<%s>(%p)]: mBase: %p, mPtr: %p, ref: %u, wref: %u",
			   TYPE_NAME(T), this, mBase, mBase ? Get() : nullptr,
			   GetRef(), GetWeakRef());
}

template <class T>
void CSharedPtr<T>::DefaultDeleter(T *buf)
{
	SPTR_DEBUG("[CSharedPtr<%s>]: DefaultDeleter", TYPE_NAME(T));

	delete buf;
}

template <class T>
void CSharedPtr<T>::ArrayDeleter(T *buf)
{
	SPTR_DEBUG("[CSharedPtr<%s>]: ArrayDeleter", TYPE_NAME(T));

	delete [] buf;
}

template <class T>
void CSharedPtr<T>::NullDeleter(T *)
{
	SPTR_DEBUG("[CSharedPtr<%s>]: NullDeleter", TYPE_NAME(T));
}

template <class T>
inline void CSharedPtr<T>::Replace(CSharedBase<T> *base)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: replace from CSharedBase<%s> *",
			   TYPE_NAME(T), this, TYPE_NAME(T));

	if (nullptr != mBase) {
		mBase->ReleaseRef();
	}

	mBase = base;

	if (nullptr != mBase) {
		SetShared();
	}

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: replace from CSharedBase<%s> *",
			   TYPE_NAME(T), this, TYPE_NAME(T));
}

template <class T>
template <class K,
		 DECLARE_ENABLE_IF(has_member_ImEnableSharedPtr<K>)>
inline void CSharedPtr<T>::SetShared(void)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: SetShared",
			   TYPE_NAME(T), this);

	if (nullptr == mBase) {
		throw ES("Set the shared base for empty CSharedPtr");
	}

	mBase->mPtr->SetSharedBase(*mBase);

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: SetShared",
			   TYPE_NAME(T), this);
}

template <class T>
template <class K,
		 DECLARE_ENABLE_IF(!has_member_ImEnableSharedPtr<K>)>
inline void CSharedPtr<T>::SetShared(void)
{
	SPTR_DEBUG("[CSharedPtr<%s>(%p)]: SetShared ignore",
			   TYPE_NAME(T), this);
}

/* Used only for meta programming. */
template <class T>
inline void CSharedPtr<T>::ImSharedPtr(void)
{
	throw ES("ImSharedPtr should not be called");
}

#include "SharedPtrOverload.hpp"

#endif /* __SHARED_PTR_HPP__ */

