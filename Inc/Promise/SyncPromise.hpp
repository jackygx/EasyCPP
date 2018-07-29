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
#include "SyncPromiseCreator.hpp"

/* Prototype of the CSyncPromiseBase */
template <class... T>
class CSyncPromise {};

/* This is an Empty SyncPromise.
 * It does not provide any method. */
template <>
class CSyncPromise<void> :
	public CPromiseBase
{
public:
	template <class... T>
	inline CSyncPromise(T && ...) :
		CPromiseBase(IGNORE)
	{
		PROMISE_DEBUG("CSyncPromiseBase<void>: Constructor");
	}
};

/* This is a succeed only SyncPromiseBase.
 * Either the promise is created without ErrorType
 * or all the ErrorTypes are handled by Catch.
 * Only Then method of this promise will be handled
 * if the previous Catch method recovers the error,
 * Otherwise, this promise will be ignored. */
template <class T>
class CSyncPromise<T> :
	public CPromiseBase,
	public CThenable
{
	typedef TO_PROMISABLE_TYPE(T) ParamType;

public:
	/**********************************************************
	 * If the previous Catch method does not reocver the error,
	 * This constructor is provided to create an ignored Promise.
	 * The ignored promise will not call Then/Catch method and
	 * it will only keep creating other ignored Promise.
	 **********************************************************/

	/* ParamType can be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ParamType),
			 ENABLE_IF(has_constructor<_ParamType, std::nullptr_t>)>
	inline CSyncPromise(const Ignore &) :
		CPromiseBase(IGNORE),
		mParams(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<%s>[%d] constructor",
					  TYPE_NAME(ParamType),
					  mPromiseType);
	}

	/* ParamType cannot be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ParamType),
			 ENABLE_IF(!has_constructor<_ParamType, std::nullptr_t>)>
	inline CSyncPromise(const Ignore &) :
		CPromiseBase(IGNORE)
	{
		PROMISE_DEBUG("CSyncPromise<%s>[%d] constructor",
					  TYPE_NAME(ParamType),
					  mPromiseType);
	}

	/*************************************************
	 * Succeed case.
	 *************************************************/
	inline CSyncPromise(const ParamType &params, CPromiseBase::Type = SUCCEED) :
		CPromiseBase(SUCCEED),
		mParams(params)
	{
		PROMISE_DEBUG("CSyncPromise<%s>[%d] constructor",
					  TYPE_NAME(ParamType),
					  mPromiseType);
	}

	/*********************************************************
	 * If ParamType is thenable, Then method is provided.
	 **********************************************************/
	template <class Fn,
			 TYPE_ALIAS(_ParamType, ParamType),
			 ENABLE_IF(IS_THENABLE(_ParamType)),
			 TYPE_ALIAS(RetType, FN_RET_TYPE(Fn)),
			 TYPE_ALIAS(PromisePtr, CSharedPtr<CSyncPromise<RetType>>)>
	inline PromisePtr Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise<%s>[%d] Then",
					  TYPE_NAME(ParamType),
					  mPromiseType);

		switch (mPromiseType) {
		case SUCCEED:
			return ThenCreatePromise<PromisePtr>(mParams, fn);

		default:
			return PromisePtr(CPromiseBase::Ignore());
		}
	}

	/*********************************************************
	 * If ParamType is not thenable, this promise can be
	 * implicitly converted into the ParamType.
	 **********************************************************/
	template <class Fn,
			  TYPE_ALIAS(_ParamType, ParamType),
			  ENABLE_IF(!IS_THENABLE(_ParamType))>
	inline _ParamType Convert(void) const
	{
		switch (mPromiseType) {
		case SUCCEED:
			return mParams;

		default:
			throw ES("Some promise error is not handled");
		}
	}

private:
	ParamType mParams;
};

/* This is a fail only SyncPromiseBase.
 * The Then method of the previous promise
 * did not return so there will be only ErrorType. */
template <class T>
class CSyncPromise<void, T> :
	public CPromiseBase,
	public CThenable
{
	typedef TO_PROMISABLE_TYPE(T) ErrorType;

public:
	/**********************************************************
	 * If the previous Catch method does not reocver the error,
	 * This constructor is provided to create an ignored Promise.
	 * The ignored promise will not call Then/Catch method and
	 * it will only keep creating other ignored Promise.
	 **********************************************************/

	/* ErrorType can be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ErrorType),
			 ENABLE_IF(has_constructor<_ParamType, std::nullptr_t>)>
	inline CSyncPromise(const Ignore &) :
		CPromiseBase(IGNORE),
		mErrors(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<void, %s>[%d] constructor",
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/* ErrorType cannot be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ErrorType),
			 ENABLE_IF(!has_constructor<_ParamType, std::nullptr_t>)>
	inline CSyncPromise(const Ignore &) :
		CPromiseBase(IGNORE)
	{
		PROMISE_DEBUG("CSyncPromise<void, %s>[%d] constructor",
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/*************************************************
	 * Succeed case.
	 *************************************************/

	/* ErrorType can be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ErrorType),
			 ENABLE_IF(has_constructor<_ParamType, std::nullptr_t>)>
	inline CSyncPromise(CPromiseBase::Type) :
		CPromiseBase(SUCCEED),
		mErrors(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<void, %s>[%d] constructor",
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/* ErrorType can be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ErrorType),
			 ENABLE_IF(!has_constructor<_ParamType, std::nullptr_t>)>
	inline CSyncPromise(CPromiseBase::Type) :
		CPromiseBase(SUCCEED)
	{
		PROMISE_DEBUG("CSyncPromise<void, %s>[%d] constructor",
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/*************************************************
	 * Fail case.
	 *************************************************/
	inline CSyncPromise(const ErrorType &errors, CPromiseBase::Type = FAIL) :
		CPromiseBase(FAIL),
		mErrors(errors)
	{
		PROMISE_DEBUG("CSyncPromise<void, %s>[%d] constructor",
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/*********************************************************
	 * If ErrorType is catchable, Catch method is provided.
	 **********************************************************/

	/* Remove the ErrorType and let Catch method handle it */
	template <class Fn,
			 TYPE_ALIAS(_ErrorType, ErrorType),
			 ENABLE_IF(IS_CATCHABLE(_ErrorType))>
	inline void Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise<void, %s>[%d] constructor",
					  TYPE_NAME(ErrorType),
					  mPromiseType);

		switch (mPromiseType) {
		case FAIL:
			mErrors->Catch(fn);

		default:
			return;
		}
	}

private:
	ErrorType mErrors;
};


/* SyncPromise has ParamType and ErrorType.
 * Then method is used to handle ParamType.
 * Catch method is used to handle ErrorType. */
template <class T, class E>
class CSyncPromise<T, E> :
	public CPromiseBase,
	public CThenable,
	public CCatchable
{
	typedef TO_PROMISABLE_TYPE(T) ParamType;
	typedef TO_PROMISABLE_TYPE(E) ErrorType;

public:
	/**********************************************************
	 * If the previous Catch method does not reocver the error,
	 * we provide a method to create a Ignore Promise.
	 * The Ignore promise will not call Then/Catch method and
	 * it will only keep creating other Ignore Promise.
	 **********************************************************/

	/* Both ParamType and ErrorType can be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ParamType),
			 TYPE_ALIAS(_ErrorType, ErrorType),
			 ENABLE_IF(has_constructor<_ParamType, std::nullptr_t>),
			 ENABLE_IF(has_constructor<_ErrorType, std::nullptr_t>)>
	inline CSyncPromise(const Ignore &) :
		CPromiseBase(IGNORE),
		mParams(nullptr),
		mErrors(nullptr)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/* ErrorType can be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ParamType),
			 TYPE_ALIAS(_ErrorType, ErrorType),
			 ENABLE_IF(!has_constructor<_ParamType, std::nullptr_t>),
			 ENABLE_IF(has_constructor<_ErrorType, std::nullptr_t>)>
	inline CSyncPromise(const Ignore &) :
		CPromiseBase(IGNORE),
		mErrors(nullptr)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/* ParamType can be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ParamType),
			 TYPE_ALIAS(_ErrorType, ErrorType),
			 ENABLE_IF(has_constructor<_ParamType, std::nullptr_t>),
			 ENABLE_IF(!has_constructor<_ErrorType, std::nullptr_t>)>
	inline CSyncPromise(const Ignore &) :
		CPromiseBase(IGNORE),
		mParams(nullptr)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/* Neither ParamType nor ErrorType can be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ParamType),
			 TYPE_ALIAS(_ErrorType, ErrorType),
			 ENABLE_IF(!has_constructor<_ParamType, std::nullptr_t>),
			 ENABLE_IF(!has_constructor<_ErrorType, std::nullptr_t>)>
	inline CSyncPromise(const Ignore &) :
		CPromiseBase(IGNORE)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/*************************************************
	 * ParamType and ErrorType are the same type,
	 * PromiseType will be used to decide if the promise
	 * is succeeded or failed.
	 *************************************************/
	template <TYPE_ALIAS(_ParamType, ParamType),
			 TYPE_ALIAS(_ErrorType, ErrorType),
			 ENABLE_IF(std::is_same<_ParamType, _ErrorType>)>
	inline CSyncPromise(const _ParamType &params, CPromiseBase::Type type) :
		CPromiseBase(type),
		mParams(params),
		mErrors(params)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/*************************************************
	 * ParamType and ErrorType are the different type,
	 * Promise status is defined by the input parameter.
	 *************************************************/

	/* Input ParamType.
	 * ErrorType can be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ParamType),
			 TYPE_ALIAS(_ErrorType, ErrorType),
			 ENABLE_IF(!std::is_same<_ParamType, _ErrorType>),
			 ENABLE_IF(has_constructor<_ErrorType, std::nullptr_t>)>
	inline CSyncPromise(const _ParamType &params, CPromiseBase::Type = SUCCEED) :
		CPromiseBase(SUCCEED),
		mParams(params),
		mErrors(nullptr)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/* Input ParamType.
	 * ErrorType cannot be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ParamType),
			 TYPE_ALIAS(_ErrorType, ErrorType),
			 ENABLE_IF(!std::is_same<_ParamType, _ErrorType>),
			 ENABLE_IF(!has_constructor<_ErrorType, std::nullptr_t>)>
	inline CSyncPromise(const _ParamType &params, CPromiseBase::Type = SUCCEED) :
		CPromiseBase(SUCCEED),
		mParams(params)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/* Input ErrorType.
	 * ParamType can be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ParamType),
			 TYPE_ALIAS(_ErrorType, ErrorType),
			 ENABLE_IF(!std::is_same<_ParamType, _ErrorType>),
			 ENABLE_IF(has_constructor<_ParamType, std::nullptr_t>)>
	inline CSyncPromise(const _ErrorType &errors, CPromiseBase::Type = FAIL) :
		CPromiseBase(FAIL),
		mParams(nullptr),
		mErrors(errors)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/* Input ErrorType.
	 * ParamType cannot be constructed by nullptr */
	template <TYPE_ALIAS(_ParamType, ParamType),
			 TYPE_ALIAS(_ErrorType, ErrorType),
			 ENABLE_IF(!std::is_same<_ParamType, _ErrorType>),
			 ENABLE_IF(!has_constructor<_ParamType, std::nullptr_t>)>
	inline CSyncPromise(const _ErrorType &errors, CPromiseBase::Type = FAIL) :
		CPromiseBase(FAIL),
		mErrors(errors)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/*********************************************************
	 * If ParamType is thenable, Then method is provided.
	 **********************************************************/

	/* Append the ErrorType to the return type
	 * regardless if the return type is a promise or not */
	template <class Fn,
			 TYPE_ALIAS(_ParamType, ParamType),
			 TYPE_ALIAS(_ErrorType, ErrorType),
			 ENABLE_IF(IS_THENABLE(_ParamType)),
			 TYPE_ALIAS(RetType, FN_RET_TYPE(Fn)),
			 TYPE_ALIAS(PromisePtr, CSharedPtr<CSyncPromise<RetType, _ErrorType>>)>
	inline PromisePtr Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Then",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);

		switch (mPromiseType) {
		case SUCCEED:
			return ThenCreatePromise<PromisePtr>(mParams, fn);

		case FAIL:
			return PromisePtr(mErrors, FAIL);

		case IGNORE:
		default:
			return PromisePtr(CPromiseBase::Ignore());
		}
	}

	/*********************************************************
	 * If ErrorType is catchable, Catch method is provided.
	 **********************************************************/

	/* If ParamType is a promise, return it directly */
	template <class Fn,
			 TYPE_ALIAS(_ParamType, ParamType),
			 TYPE_ALIAS(_ErrorType, ErrorType),
			 ENABLE_IF(IS_CATCHABLE(_ErrorType)),
			 ENABLE_IF(IS_PROMISABLE(_ParamType)),
			 TYPE_ALIAS(PromisePtr, ParamType)>
	inline PromisePtr Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Catch",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);

		switch (mPromiseType) {
		case SUCCEED:
			return mParams;

		case FAIL:
			return CatchCreatePromise<PromisePtr>(mErrors, fn);

		case IGNORE:
		default:
			return PromisePtr(CPromiseBase::Ignore());
		}
	}

	/* If ParamType is a promise, create the CSyncPromise<ParamType> */
	template <class Fn,
			 TYPE_ALIAS(_ParamType, ParamType),
			 TYPE_ALIAS(_ErrorType, ErrorType),
			 ENABLE_IF(!IS_PROMISABLE(_ParamType)),
			 ENABLE_IF(IS_CATCHABLE(_ErrorType)),
			 TYPE_ALIAS(PromisePtr, CSharedPtr<CSyncPromise<_ParamType>>)>
	inline PromisePtr Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Catch",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);

		switch (mPromiseType) {
		case SUCCEED:
			return PromisePtr(mParams, SUCCEED);

		case FAIL:
			return CatchCreatePromise<PromisePtr>(mErrors, fn);

		case IGNORE:
		default:
			return PromisePtr(CPromiseBase::Ignore());
		}
	}

private:
	ParamType mParams;
	ErrorType mErrors;
};

#endif /* __SYNC_PROMISE_BASE_HPP__ */

