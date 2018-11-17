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

#ifndef __SYNC_PROMISE_HPP__
#define __SYNC_PROMISE_HPP__

#include "SyncPromiseBase.hpp"

/**********************************************************
 * <Then> Only SyncPromise:
 *
 * 1. The SyncPromise is created without <CatchType>.
 * 2. The <CatchType> is handled by the <Catch>.
 **********************************************************/
template <class... Tn>
class CSyncPromise<CSharedPtr<CSyncPromiseThenParams<Tn...>>> :
	public CSyncPromiseBase<CSharedPtr<CSyncPromiseThenParams<Tn...>>>
{
	typedef CSharedPtr<CSyncPromiseThenParams<Tn...>> ThenType;
	typedef CSyncPromiseBase<ThenType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const ThenType &params, CPromiseBase::Type type) :
		Parent(params, type)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const Tn & ... tn, CPromiseBase::Type type) :
		CPromiseBase(ThenType(tn...), type)
	{
		/* Does nothing */
	}
};

/* specialization: void */
template <>
class CSyncPromise<CSharedPtr<CSyncPromiseThenParams<void>>> :
	public CSyncPromiseBase<CSharedPtr<CSyncPromiseThenParams<void>>>
{
	typedef CSharedPtr<CSyncPromiseThenParams<void>> ThenType;
	typedef CSyncPromiseBase<ThenType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const ThenType &params, CPromiseBase::Type type) :
		Parent(params, type)
	{
		/* Does nothing */
	}

	inline CSyncPromise(CPromiseBase::Type type) :
		Parent(ThenType(), type)
	{
		/* Does nothing */
	}
};


/**********************************************************
 * <Catch> Only SyncPromise:
 *
 * 1. When the <Then> method of the previous SyncPromise
 *    returns <void>.
 **********************************************************/
template <class... Cn>
class CSyncPromise<CSharedPtr<CSyncPromiseCatchParams<Cn...>>> :
	public CSyncPromiseBase<CSharedPtr<CSyncPromiseCatchParams<Cn...>>>
{
	typedef CSharedPtr<CSyncPromiseCatchParams<Cn...>> CatchType;
	typedef CSyncPromiseBase<CatchType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const CatchType &params, CPromiseBase::Type type) :
		CPromiseBase(params, type)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const Cn & ... cn, CPromiseBase::Type type) :
		CPromiseBase(CatchType(cn...), type)
	{
		/* Does nothing */
	}
};

/* specialization: void */
template <>
class CSyncPromise<CSharedPtr<CSyncPromiseCatchParams<void>>> :
	public CSyncPromiseBase<CSharedPtr<CSyncPromiseCatchParams<void>>>
{
	typedef CSharedPtr<CSyncPromiseCatchParams<void>> CatchType;
	typedef CSyncPromiseBase<CatchType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const CatchType &params, CPromiseBase::Type type) :
		Parent(params, type)
	{
		/* Does nothing */
	}

	inline CSyncPromise(CPromiseBase::Type type) :
		Parent(CatchType(), type)
	{
		/* Does nothing */
	}
};


/**********************************************************
 * A SyncPromise has both ThenType and CatchType.
 *
 * ThenType and CatchType are different type.
 *
 * <Then>  method is used to handle ThenType.
 * <Catch> method is used to handle CatchType.
 **********************************************************/
template <class... Tn, class... Cn>
class CSyncPromise<CSharedPtr<CSyncPromiseThenParams<Tn...>>,
				   CSharedPtr<CSyncPromiseCatchParams<Cn...>>> :
	public CSyncPromiseBase<CSharedPtr<CSyncPromiseThenParams<Tn...>>,
							CSharedPtr<CSyncPromiseCatchParams<Cn...>>>
{
	typedef CSharedPtr<CSyncPromiseThenParams<Tn...>> ThenType;
	typedef CSharedPtr<CSyncPromiseCatchParams<Cn...>> CatchType;
	typedef CSyncPromiseBase<ThenType, CatchType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const ThenType &params, CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(params)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const Tn & ... tn, CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(ThenType(tn...))
	{
		/* Does nothing */
	}

	inline CSyncPromise(const CatchType &params, CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(params)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const Cn & ... cn, CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(CatchType(cn...))
	{
		/* Does nothing */
	}
};


/* specialization: Same type for <Then> and <Catch> */
template <class... Tn>
class CSyncPromise<CSharedPtr<CSyncPromiseThenParams<Tn...>>,
				   CSharedPtr<CSyncPromiseCatchParams<Tn...>>> :
	public CSyncPromiseBase<CSharedPtr<CSyncPromiseThenParams<Tn...>>,
							CSharedPtr<CSyncPromiseCatchParams<Tn...>>>
{
	typedef CSharedPtr<CSyncPromiseThenParams<Tn...>> ThenType;
	typedef CSharedPtr<CSyncPromiseCatchParams<Tn...>> CatchType;
	typedef CSyncPromiseBase<ThenType, CatchType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const ThenType &params, CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(params)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const CatchType &params, CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(params)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const Tn & ... tn, CPromiseBase::Type type) :
		Parent(type)
	{
		if (CPromiseBase::SUCCEED == type) {
			Parent::SetThenParams(ThenType(tn...));
		} else {
			Parent::SetCatchParams(CatchType(tn...));
		}
	}
};


/* specialization: <Catch> is void */
template <class... Tn>
class CSyncPromise<CSharedPtr<CSyncPromiseThenParams<Tn...>>,
				   CSharedPtr<CSyncPromiseCatchParams<void>>> :
	public CSyncPromiseBase<CSharedPtr<CSyncPromiseThenParams<Tn...>>,
							CSharedPtr<CSyncPromiseCatchParams<void>>>
{
	typedef CSharedPtr<CSyncPromiseThenParams<Tn...>> ThenType;
	typedef CSharedPtr<CSyncPromiseCatchParams<void>> CatchType;
	typedef CSyncPromiseBase<ThenType, CatchType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const ThenType &params, CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(params)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const Tn & ... tn, CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(ThenType(tn...))
	{
		/* Does nothing */
	}

	inline CSyncPromise(const CatchType &params, CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(params)
	{
		/* Does nothing */
	}

	inline CSyncPromise(CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(CatchType())
	{
		/* Does nothing */
	}
};


/* specialization: <Then> is void */
template <class... Cn>
class CSyncPromise<CSharedPtr<CSyncPromiseThenParams<void>>,
				   CSharedPtr<CSyncPromiseCatchParams<Cn...>>> :
	public CSyncPromiseBase<CSharedPtr<CSyncPromiseThenParams<void>>,
							CSharedPtr<CSyncPromiseCatchParams<Cn...>>>
{
	typedef CSharedPtr<CSyncPromiseThenParams<void>> ThenType;
	typedef CSharedPtr<CSyncPromiseCatchParams<Cn...>> CatchType;
	typedef CSyncPromiseBase<ThenType, CatchType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const ThenType &params, CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(params)
	{
		/* Does nothing */
	}

	inline CSyncPromise(CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(ThenType())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const CatchType &params, CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(params)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const Cn & ... cn, CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(CatchType(cn...))
	{
		/* Does nothing */
	}
};


/* specialization: Both <Then> and <Catch> are void */
template <>
class CSyncPromise<CSharedPtr<CSyncPromiseThenParams<void>>,
				   CSharedPtr<CSyncPromiseCatchParams<void>>> :
	public CSyncPromiseBase<CSharedPtr<CSyncPromiseThenParams<void>>,
							CSharedPtr<CSyncPromiseCatchParams<void>>>
{
	typedef CSharedPtr<CSyncPromiseThenParams<void>> ThenType;
	typedef CSharedPtr<CSyncPromiseCatchParams<void>> CatchType;
	typedef CSyncPromiseBase<ThenType, CatchType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const ThenType &params, CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(params)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const CatchType &params, CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(params)
	{
		/* Does nothing */
	}

	inline CSyncPromise(CPromiseBase::Type type) :
		Parent(type)
	{
		if (CPromiseBase::SUCCEED == type) {
			Parent::SetThenParams(ThenType());
		} else {
			Parent::SetCatchParams(CatchType());
		}
	}
};


/***************************************************************************
 * Help MACRO to declare a SyncPromise<>.
 * Format:
 * 1. Declare SyncPromise.
 *    DEFINE_SYNC_PROMISE(name, (Then params...) , (Catch params...))
 *    CSyncPromise<CsharedPtr<CSyncPromiseParams<Then params...>>,
 *                 CSharedPtr<CSyncPromiseParams<Catch params...>>>
 *
 * 2. Declare <Then> only SyncPromise:
 *    DEFINE_SYNC_PROMISE(name, (Then params...))
 *
 * Example:
 * DEFINE_SYNC_PROMISE(Example, (int, int *), (float, float *))
 * Declare a SyncPromise called CExamplePromisePtr.
 * The <Then>  method accept a callback function: Fn(int, int*);
 * The <Catch> method accept a callback function: Fn(float, float*);
 ***************************************************************************/
#define THEN_PROMISE_PARAM_TYPE(...) \
	CSharedPtr<CSyncPromiseThenParams<__VA_ARGS__>>

#define CATCH_PROMISE_PARAM_TYPE(...) \
	CSharedPtr<CSyncPromiseCatchParams<__VA_ARGS__>>

#define PROMISE_PACK_TYPE(...) \
	pack<__VA_ARGS__>

#define DEFINE_SYNC_PROMISE_1(name, thenParams) \
	typedef THEN_PROMISE_PARAM_TYPE thenParams C##name##PromiseThenPtr; \
	typedef CSharedPtr<CSyncPromise<C##name##PromiseThenPtr>> C##name##PromisePtr

#define DEFINE_SYNC_PROMISE_2(name, thenParams, catchParams) \
	typedef THEN_PROMISE_PARAM_TYPE thenParams C##name##PromiseThenPtr; \
	typedef CATCH_PROMISE_PARAM_TYPE catchParams C##name##PromiseCatchPtr; \
	typedef CSharedPtr<CSyncPromise<C##name##PromiseThenPtr, \
									C##name##PromiseCatchPtr>> \
									C##name##PromisePtr

#define DEFINE_SYNC_PROMISE(name, ...) \
	CONNECT(DEFINE_SYNC_PROMISE_, NARG(__VA_ARGS__))(name, __VA_ARGS__)


#if 0
/***************************************************************************
 * Help FUNCTION to create a SyncPromise proxy.
 * Parameters:
 * 1. First parameter must be a SyncPromise.
 * 2. Rest parameter will be passed to the Fn when <Then> method is called.
 ***************************************************************************/
template <class T,
		 class... Tn>
inline decltype(auto) CreateSyncPromise(T &&t, Tn && ... tn)
{
	typedef CSharedPtr<CSyncPromiseParams<T, Tn...>> ThenType;
	using ErrorType = decltype(t->GetSyncPromiseError());
	typedef CSharedPtr<CSyncPromise<ThenType, ErrorType>> PromisePtr;

	if (t) {
		return PromisePtr(ThenType(t, tn...));
	} else {
		return PromisePtr(t->GetSyncPromiseError());
	}
}
#endif

#endif /* __SYNC_PROMISE_HPP__ */

