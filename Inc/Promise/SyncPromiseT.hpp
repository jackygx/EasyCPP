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

/* T can be:
 * SyncPromise,
 * void,
 * Other type.
 */

#ifndef __SYNC_PROMISE_T_HPP__
#define __SYNC_PROMISE_T_HPP__

#include "PromiseParams.hpp"
#include "SyncPromiseHeader.hpp"

/* Succeed case: SyncPromise.
 * Fail case:    N/A. */
template <class... Tn>
class CSyncPromise<CSharedPtr<CSyncPromise<Tn...>>> :
	public CPromisable
{
	typedef CSharedPtr<CSyncPromise<Tn...>> ParamType;

public:
	/* the error comes from the previous promise.
	 * So we will ignore Then or Catch in this promise.
	 * Just pass the error to next promise. */
	inline CSyncPromise(const CPromiseFail &) :
		mParams(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<%s> ignore", TYPE_NAME(ParamType));
	}

	/* Succeed case */
	inline CSyncPromise(const ParamType &promise) :
		mParams(promise)
	{
		PROMISE_DEBUG("CSyncPromise<%s> succeed", TYPE_NAME(ParamType));
	}

	template <class Fn>
	inline decltype(auto) Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise<%s> Then", TYPE_NAME(ParamType));

		typedef CSharedPtr<CSyncPromise<FN_RET_TYPE(Fn)>> PromisePtr;

		/* Succeed case */
		if (mParams) {
			return PromisePtr(mParams->Then(fn));

		/* Ignore case */
		} else {
			return PromisePtr(CPromiseFail());
		}
	}

private:
	ParamType mParams;
};

/* Succeed case: promise.
 * Fail case:    promise parameter. */
template <class... Tn, class... En>
class CSyncPromise<CSharedPtr<CSyncPromise<Tn...>>,
				   CSharedPtr<CPromiseParams<En...>>> :
	public CPromisable
{
	typedef CSharedPtr<CSyncPromise<Tn...>> ParamType;
	typedef CSharedPtr<CPromiseParams<En...>> ErrorType;

public:
	/* the error comes from the previous promise.
	 * So we will ignore Then or Catch in this promise.
	 * Just pass the error to next promise. */
	inline CSyncPromise(const CPromiseFail &) :
		mParams(nullptr),
		mErrors(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<%s, %s> ignore",
					  TYPE_NAME(ParamType), TYPE_NAME(ErrorType));
	}

	/* Succeed case */
	inline CSyncPromise(const ParamType &promise, bool = true) :
		mParams(promise),
		mErrors(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<%s, %s> succeed",
					  TYPE_NAME(ParamType), TYPE_NAME(ErrorType));
	}

	/* Failure case. */
	inline CSyncPromise(const ErrorType &errors, bool = false) :
		mParams(nullptr),
		mErrors(errors)
	{
		PROMISE_DEBUG("CSyncPromise<%s, %s> fail",
					  TYPE_NAME(ParamType), TYPE_NAME(ErrorType));
	}

	/* We need to append the ErrorType to the Promise
	 * until it is catched. */
	template <class Fn>
	inline decltype(auto) Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise<%s, %s> Then",
					  TYPE_NAME(ParamType), TYPE_NAME(ErrorType));

		using RetType = decltype(mParams->Then(fn));
		typedef CSharedPtr<CSyncPromise<RetType, ErrorType>> PromisePtr;

		/* Succeed case */
		if (mParams) {
			return PromisePtr(mParams->Then(fn), true);

		/* Fail case */
		} else if (mErrors) {
			return PromisePtr(mErrors, false);

		/* Ignore case */
		} else {
			return PromisePtr(CPromiseFail());
		}
	}

	/* Now the error is catched, we can remove the [ErrorType] */
	template <class Fn>
	inline decltype(auto) Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise<%s, %s> Catch",
					  TYPE_NAME(ParamType), TYPE_NAME(ErrorType));

		/* ParamType is a promise */
		typedef ParamType PromisePtr;

		/* Succeed case */
		if (mParams) {
			return mParams;

		/* Fail case */
		} else if (mErrors) {
			CATCH_TO_PROMISE(PromisePtr, mErrors, fn);

		/* Ignore case */
		} else {
			return PromisePtr(CPromiseFail());
		}
	}

private:
	ParamType mParams;
	ErrorType mErrors;
};

/* Succeed case: void.
 * Fail case:    N/A. */
template <>
class CSyncPromise<void> :
	public CPromisable
{
public:
	inline CSyncPromise(const CPromiseFail &)
	{
		PROMISE_DEBUG("CSyncPromise<void> ignore");
	}

	/* Succeed case */
	inline CSyncPromise(void)
	{
		PROMISE_DEBUG("CSyncPromise<void> succeed");
	}

	/* No Then.
	 * If someone does not return, it is the end of the chain */
};

/* Succeed case: void.
 * Fail case:    promise parameter. */
template <class... En>
class CSyncPromise<void, CSharedPtr<CPromiseParams<En...>>> :
	public CPromisable
{
	typedef CSharedPtr<CPromiseParams<En...>> ErrorType;

public:
	/* the error comes from the previous promise.
	 * So we will ignore Then or Catch in this promise.
	 * Just pass the error to next promise. */
	inline CSyncPromise(const CPromiseFail &) :
		mErrors(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<void, %s> ignore", TYPE_NAME(ErrorType));
	}

	/* Succeed case */
	inline CSyncPromise(bool = true) :
		mErrors(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<void, %s> succeed", TYPE_NAME(ErrorType));
	}

	/* Failure case. */
	inline CSyncPromise(const ErrorType &errors, bool = false) :
		mErrors(errors)
	{
		PROMISE_DEBUG("CSyncPromise<void, %s> fail", TYPE_NAME(ErrorType));
	}

	/* No Then.
	 * If someone does not return, it is the end of the chain. */

	/* Process the error and no return because it is the end of the chain. */
	template <class Fn>
	inline void Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise<void, %s> Catch", TYPE_NAME(ErrorType));

		/* Fail case */
		if (mErrors) {
			mErrors->Run(fn);
		}
	}

private:
	ErrorType mErrors;
};

/* Succeed case: value.
 * Fail case:    N/A. */
template <class T>
class CSyncPromise<T> :
	public CPromisable
{
	typedef PROMISABLE_TYPE(T) ParamType;

public:
	/* the error comes from the previous promise.
	 * So we will ignore Then or Catch in this promise.
	 * Just pass the error to next promise. */
	template <class _T = ParamType,
			 ENABLE_IF(std::is_same<_T, ParamType>),
			 ENABLE_IF(has_constructor<_T, std::nullptr_t>)>
	inline CSyncPromise(const CPromiseFail &) :
		mResult(false),
		mParams(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<%s> ignore", TYPE_NAME(ParamType));
	}

	template <class _T = ParamType,
			 ENABLE_IF(std::is_same<_T, ParamType>),
			 ENABLE_IF(!has_constructor<_T, std::nullptr_t>)>
	inline CSyncPromise(const CPromiseFail &) :
		mResult(false)
	{
		PROMISE_DEBUG("CSyncPromise<%s> ignore", TYPE_NAME(ParamType));
	}

	/* Succeed case */
	inline CSyncPromise(const ParamType &promise) :
		mResult(true),
		mParams(promise)
	{
		PROMISE_DEBUG("CSyncPromise<%s> succeed", TYPE_NAME(ParamType));
	}

	/* Implicitly converted to the ParamType */
	template <class _T = ParamType>
	inline _T Convert(void) const
	{
		if (mResult) {
			return mParams;
		} else {
			throw ES("Some promise error is not handled");
		}
	}

private:
	bool mResult;
	ParamType mParams;
};

/* Succeed case: T.
 * Fail case:    promise parameter. */
template <class T, class... En>
class CSyncPromise<T, CSharedPtr<CPromiseParams<En...>>> :
	public CPromisable
{
	typedef PROMISABLE_TYPE(T) ParamType;
	typedef CSharedPtr<CPromiseParams<En...>> ErrorType;

public:
	/* the error comes from the previous promise.
	 * So we will ignore Then or Catch in this promise.
	 * Just pass the error to next promise. */
	template <class _T = ParamType,
			 ENABLE_IF(std::is_same<_T, ParamType>),
			 ENABLE_IF(has_constructor<_T, std::nullptr_t>)>
	inline CSyncPromise(const CPromiseFail &) :
		mResult(false),
		mParams(nullptr),
		mErrors(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<%s> ignore", TYPE_NAME(ParamType));
	}

	template <class _T = ParamType,
			 ENABLE_IF(std::is_same<_T, ParamType>),
			 ENABLE_IF(!has_constructor<_T, std::nullptr_t>)>
	inline CSyncPromise(const CPromiseFail &) :
		mResult(false),
		mErrors(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<%s> ignore", TYPE_NAME(ParamType));
	}

	/* Succeed case */
	inline CSyncPromise(const ParamType &promise, bool = true) :
		mResult(true),
		mParams(promise),
		mErrors(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<%s, %s> succeed",
					  TYPE_NAME(ParamType), TYPE_NAME(ErrorType));
	}

	/* Failure case. */
	template <class _T = ParamType,
			 ENABLE_IF(std::is_same<_T, ParamType>),
			 ENABLE_IF(has_constructor<_T, std::nullptr_t>)>
	inline CSyncPromise(const ErrorType &errors, bool = false) :
		mResult(false),
		mParams(nullptr),
		mErrors(errors)
	{
		PROMISE_DEBUG("CSyncPromise<%s, %s> fail",
					  TYPE_NAME(ParamType), TYPE_NAME(ErrorType));
	}

	template <class _T = ParamType,
			 ENABLE_IF(std::is_same<_T, ParamType>),
			 ENABLE_IF(!has_constructor<_T, std::nullptr_t>)>
	inline CSyncPromise(const ErrorType &errors, bool = false) :
		mResult(false),
		mErrors(errors)
	{
		PROMISE_DEBUG("CSyncPromise<%s, %s> fail",
					  TYPE_NAME(ParamType), TYPE_NAME(ErrorType));
	}

	/* Now the error is catched, we can remove the [ErrorType] */
	template <class Fn>
	inline decltype(auto) Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise<%s, %s> Catch",
					  TYPE_NAME(ParamType), TYPE_NAME(ErrorType));

		typedef CSharedPtr<CSyncPromise<ParamType>> PromisePtr;

		/* Succeed case */
		if (mResult) {
			return PromisePtr(mParams);

		/* Fail case */
		} else if (mErrors) {
			CATCH_TO_PROMISE(PromisePtr, mErrors, fn);

		/* Ignore case */
		} else {
			return PromisePtr(CPromiseFail());
		}
	}

private:
	bool mResult;
	ParamType mParams;
	ErrorType mErrors;
};

/* Succeed case: promise, void or T.
 * Fail case:    multi parameters. */
template <class T, class... En>
class CSyncPromise<T, pack<En...>> :
	public CSyncPromise<PROMISABLE_TYPE(T),
						CSharedPtr<CPromiseParams<En...>>>
{
	typedef PROMISABLE_TYPE(T) ParamType;
	typedef CSharedPtr<CPromiseParams<En...>> ErrorType;
	typedef CSyncPromise<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const CPromiseFail &fail) :
		Parent(fail)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const ParamType &promise, bool = true) :
		Parent(promise, true)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const En & ... en, bool = false) :
		Parent(ErrorType(en...), false)
	{
		/* Does nothing */
	}
};

/* Succeed case: promise, void or T.
 * Fail case:    void. */
template <class T>
class CSyncPromise<T, pack<void>> :
	public CSyncPromise<PROMISABLE_TYPE(T),
						CSharedPtr<CPromiseParams<void>>>
{
	typedef PROMISABLE_TYPE(T) ParamType;
	typedef CSharedPtr<CPromiseParams<void>> ErrorType;
	typedef CSyncPromise<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const CPromiseFail &fail) :
		Parent(fail)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const ParamType &promise, bool = true) :
		Parent(promise, true)
	{
		/* Does nothing */
	}

	inline CSyncPromise(bool = false) :
		Parent(ErrorType(), false)
	{
		/* Does nothing */
	}
};

#endif /* __SYNC_PROMISE_T_HPP__ */

