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

#ifndef __SYNC_PROMISE_BASE_HPP__
#define __SYNC_PROMISE_BASE_HPP__

#include "PromiseParams.hpp"
#include "SyncPromiseT.hpp"

/* Succeed case: promise parameter.
 * Fail case:    N/A. */
template <class... Tn>
class CSyncPromise<CSharedPtr<CPromiseParams<Tn...>>> :
	public CPromisable
{
	typedef CSharedPtr<CPromiseParams<Tn...>> ParamType;

public:
	/* the error comes from the previous promise.
	 * So we will ignore Then or Catch in this promise.
	 * Just pass the error to next promise. */
	inline CSyncPromise(const CPromiseFail) :
		mParams(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<%s> ignore", TYPE_NAME(ParamType));
	}

	/* Succeed case */
	inline CSyncPromise(const ParamType &params) :
		mParams(params)
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
			return PromisePtr(mParams->Run(fn));

		/* Ignore case */
		} else {
			return PromisePtr(CPromiseFail());
		}
	}

private:
	ParamType mParams;
};

/* Succeed case: multiple parameters.
 * Fail case:    N/A. */
template <class... Tn>
class CSyncPromise<pack<Tn...>> :
	public CSyncPromise<CSharedPtr<CPromiseParams<Tn...>>>
{
	typedef CSharedPtr<CPromiseParams<Tn...>> ParamType;
	typedef CSyncPromise<ParamType> Parent;

public:
	inline CSyncPromise(const CPromiseFail &fail) :
		Parent(fail)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const Tn & ... tn) :
		Parent(ParamType(tn...))
	{
		/* Does nothing */
	}
};

/* Succeed case: void.
 * Fail case:    N/A. */
template <>
class CSyncPromise<pack<void>> :
	public CSyncPromise<CSharedPtr<CPromiseParams<void>>>
{
	typedef CSharedPtr<CPromiseParams<void>> ParamType;
	typedef CSyncPromise<ParamType> Parent;

public:
	inline CSyncPromise(const CPromiseFail &fail) :
		Parent(fail)
	{
		/* Does nothing */
	}

	inline CSyncPromise(void) :
		Parent(ParamType())
	{
		/* Does nothing */
	}
};

/* Succeed case: promise parameter.
 * Fail case:    promise parameter. (same type) */
template <class... Tn>
class CSyncPromise<CSharedPtr<CPromiseParams<Tn...>>,
				   CSharedPtr<CPromiseParams<Tn...>>> :
	public CPromisable
{
	typedef CSharedPtr<CPromiseParams<Tn...>> ParamType;
public:
	/* the error comes from the previous promise.
	 * So we will ignore Then or Catch in this promise.
	 * Just pass the error to next promise. */
	inline CSyncPromise(const CPromiseFail &) :
		mParams(nullptr),
		mErrors(nullptr),
		mResult(false)
	{
		PROMISE_DEBUG("CSyncPromise<%s, %s> ignore",
					  TYPE_NAME(ParamType), TYPE_NAME(ParamType));
	}

	/* Succeed/fail case */
	inline CSyncPromise(const ParamType &params, bool result) :
		mParams(params),
		mErrors(params),
		mResult(result)
	{
		PROMISE_DEBUG("CSyncPromise<%s, %s> %s",
					  TYPE_NAME(ParamType), TYPE_NAME(ParamType),
					  result ? "succeed" : "fail");
	}

	/* We need to append the ParamType to the Promise
	 * until it is catched. */
	template <class Fn>
	inline decltype(auto) Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise<%s, %s> Then",
					  TYPE_NAME(ParamType), TYPE_NAME(ParamType));

		typedef CSharedPtr<CSyncPromise<FN_RET_TYPE(Fn), ParamType>> PromisePtr;

		/* Succeed case */
		if (mResult) {
			return PromisePtr(mParams->Run(fn), true);

		/* Fail case */
		} else if (mErrors) {
			return PromisePtr(mErrors, false);

		/* Ignore case */
		} else {
			return PromisePtr(CPromiseFail());
		}
	}

	/* Now the error is catched, we can remove the [ParamType] */
	template <class Fn>
	inline decltype(auto) Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise<%s, %s> Catch",
					  TYPE_NAME(ParamType), TYPE_NAME(ParamType));

		/* From CSyncPromise<ParamType, ErrorType>
		 * to CSyncPromise<ParamType> */
		typedef CSharedPtr<CSyncPromise<ParamType>> PromisePtr;

		/* Succeed case */
		if (mResult) {
			return PromisePtr(mParams);

		/* Fail case */
		} else if (mErrors) {
			return CatchRetToPromise<PromisePtr>(mErrors->Run(fn));

		/* Ignore case */
		} else {
			return PromisePtr(CPromiseFail());
		}
	}

private:
	ParamType mParams;
	ParamType mErrors;
	bool mResult;
};

/* Succeed case: promise parameter.
 * Fail case:    promise parameter. */
template <class... Tn, class... En>
class CSyncPromise<CSharedPtr<CPromiseParams<Tn...>>,
				   CSharedPtr<CPromiseParams<En...>>> :
	public CPromisable
{
	typedef CSharedPtr<CPromiseParams<Tn...>> ParamType;
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
	inline CSyncPromise(const ParamType &params, bool = true) :
		mParams(params),
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

		typedef CSharedPtr<CSyncPromise<FN_RET_TYPE(Fn), ErrorType>> PromisePtr;

		/* Succeed case */
		if (mParams) {
			return PromisePtr(mParams->Run(fn), true);

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
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType));

		/* From CSyncPromise<ParamType, ErrorType>
		 * to CSyncPromise<ParamType> */
		typedef CSharedPtr<CSyncPromise<ParamType>> PromisePtr;

		/* Succeed case */
		if (mParams) {
			return PromisePtr(mParams);

		/* Fail case */
		} else if (mErrors) {
			return CatchRetToPromise<PromisePtr>(mErrors->Run(fn));

		/* Ignore case */
		} else {
			return PromisePtr(CPromiseFail());
		}
	}

private:
	ParamType mParams;
	ErrorType mErrors;
};

/* Succeed case: multiple parameters.
 * Fail case:    multiple parameters. */
template <class... Tn, class... En>
class CSyncPromise<pack<Tn...>, pack<En...>> :
	public CSyncPromise<CSharedPtr<CPromiseParams<Tn...>>,
						CSharedPtr<CPromiseParams<En...>>>
{
	typedef CSharedPtr<CPromiseParams<Tn...>> ParamType;
	typedef CSharedPtr<CPromiseParams<En...>> ErrorType;
	typedef CSyncPromise<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const CPromiseFail &fail) :
		Parent(fail)
	{
		/* Does nothing */
	}

	/* Succeed case */
	inline CSyncPromise(const Tn & ... tn, bool = true) :
		Parent(ParamType(tn...), true)
	{
		/* Does nothing */
	}

	/* Failure case. */
	inline CSyncPromise(const En & ... en, bool  = false) :
		Parent(ErrorType(en...), false)
	{
		/* Does nothing */
	}
};

/* Succeed case: void.
 * Fail case:    multiple parameters. */
template <class... En>
class CSyncPromise<pack<void>, pack<En...>> :
	public CSyncPromise<CSharedPtr<CPromiseParams<void>>,
						CSharedPtr<CPromiseParams<En...>>>
{
	typedef CSharedPtr<CPromiseParams<void>> ParamType;
	typedef CSharedPtr<CPromiseParams<En...>> ErrorType;
	typedef CSyncPromise<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const CPromiseFail &fail) :
		Parent(fail)
	{
		/* Does nothing */
	}

	/* Succeed case */
	inline CSyncPromise(bool = true) :
		Parent(ParamType(), true)
	{
		/* Does nothing */
	}

	/* Failure case. */
	inline CSyncPromise(const En & ... en, bool = false) :
		Parent(ErrorType(en...), false)
	{
		/* Does nothing */
	}
};

/* Succeed case: multiple parameters.
 * Fail case:    void. */
template <class... Tn>
class CSyncPromise<pack<Tn...>, pack<void>> :
	public CSyncPromise<CSharedPtr<CPromiseParams<Tn...>>,
						CSharedPtr<CPromiseParams<void>>>
{
	typedef CSharedPtr<CPromiseParams<Tn...>> ParamType;
	typedef CSharedPtr<CPromiseParams<void>> ErrorType;
	typedef CSyncPromise<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const CPromiseFail &fail) :
		Parent(fail)
	{
		/* Does nothing */
	}

	/* Succeed case */
	inline CSyncPromise(const Tn & ... tn, bool = true) :
		Parent(ParamType(tn...), true)
	{
		/* Does nothing */
	}

	/* Failure case. */
	inline CSyncPromise(bool = false) :
		Parent(ErrorType(), false)
	{
		/* Does nothing */
	}
};

/* Succeed case: void.
 * Fail case:    void. */
template <>
class CSyncPromise<pack<void>, pack<void>> :
	public CSyncPromise<CSharedPtr<CPromiseParams<void>>,
						CSharedPtr<CPromiseParams<void>>>
{
	typedef CSharedPtr<CPromiseParams<void>> ParamType;
	typedef CSyncPromise<ParamType, ParamType> Parent;

public:
	inline CSyncPromise(const CPromiseFail &fail) :
		Parent(fail)
	{
		/* Does nothing */
	}

	inline CSyncPromise(bool result) :
		Parent(ParamType(), result)
	{
		/* Does nothing */
	}
};

#endif /* __SYNC_PROMISE_BASE_HPP__ */

