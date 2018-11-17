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

#include "SyncPromiseCreator.hpp"

/* Prototype of the CSyncPromiseBase */
template <class... T>
class CSyncPromiseBase {};

/* This is an Empty SyncPromiseBase.
 * It does not provide any method. */
template <>
class CSyncPromiseBase<void> :
	public CPromiseBase
{
public:
	template <class... T>
	inline CSyncPromiseBase(T && ...) :
		CPromiseBase(IGNORE)
	{
		PROMISE_DEBUG("CSyncPromiseBase<void>: Constructor");
	}
};

/* This is a succeed only SyncPromiseBase.
 * Either the promise is created without ErrorType
 * or all the ErrorType is handled by Catch.
 * If the previous Catch method recovers the error,
 * Then method of this promise will be handled.
 * Otherwise, this promise will be ignore. */
template <class P>
class CSyncPromiseBase<P> :
	public CPromiseBase,
	public CThenable
{
	typedef TO_PROMISABLE_TYPE(P) ParamType;

public:
	/**********************************************************
	 * If the previous Catch method does not reocver the error,
	 * we provide a method to create a Ignore Promise.
	 * The Ignore promise will not call Then/Catch method and
	 * it will only keep creating other Ignore Promise.
	 **********************************************************/

	/* ParamType can be initialized by nullptr. */
	template <class T = ParamType,
			 ENABLE_IF(std::is_same<T, ParamType>),
			 ENABLE_IF(has_constructor<T, std::nullptr_t>)>
	inline CSyncPromiseBase(const PromiseIgnore &) :
		CPromiseBase(IGNORE),
		mParams(nullptr)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s>[%d]: Constructor",
					  TYPE_NAME(ParamType), mPromiseType);
	}

	/* ParamType cannot be initialized by nullptr. */
	template <class T = ParamType,
			 ENABLE_IF(std::is_same<T, ParamType>),
			 ENABLE_IF(!has_constructor<T, std::nullptr_t>)>
	inline CSyncPromiseBase(const PromiseIgnore &) :
		CPromiseBase(IGNORE)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s>[%d]: Constructor",
					  TYPE_NAME(ParamType), mPromiseType);
	}

	/*************************************************
	 * A succeed promise is created.
	 *************************************************/
	inline CSyncPromiseBase(const ParamType &params, enum PromiseType = SUCCEED) :
		CPromiseBase(SUCCEED),
		mParams(params)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s>[%d]: Constructor",
					  TYPE_NAME(ParamType), mPromiseType);
	}

	/*********************************************************
	 * If ParamType is thenable, Then method is provided.
	 **********************************************************/
	/* If the Fn returns a promisable,
	 * the promisable will be returned directly. */
	template <class Fn,
			 class T = ParamType,
			 ENABLE_IF(std::is_same<T, ParamType>),
			 ENABLE_IF(IS_THENABLE(T)),
			 class RetType = FN_RET_TYPE(Fn),
			 ENABLE_IF(std::is_same<RetType, FN_RET_TYPE(Fn)>),
			 ENABLE_IF(IS_PROMISABLE(RetType))>
	inline decltype(auto) Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s>[%d]: Then",
					  TYPE_NAME(ParamType), mPromiseType);

		switch (mPromiseType) {
		case SUCCEED:
			return mParams->Then(fn);

		case IGNORE:
		default:
			return RetType(PromiseIgnore());
		}
	}

	/* If the Fn does not return a promisable,
	 * a new promisable will be created and returned. */
	template <class Fn,
			 class T = ParamType,
			 ENABLE_IF(std::is_same<T, ParamType>),
			 ENABLE_IF(IS_THENABLE(T)),
			 class RetType = FN_RET_TYPE(Fn),
			 ENABLE_IF(std::is_same<RetType, FN_RET_TYPE(Fn)>),
			 ENABLE_IF(!IS_PROMISABLE(RetType))>
	inline decltype(auto) Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s>[%d]: Then",
					  TYPE_NAME(ParamType), mPromiseType);

		typedef CSharedPtr<CSyncPromiseBase<RetType>> PromisePtr;

		switch (mPromiseType) {
		case SUCCEED:
			return ThenCreatePromise<PromisePtr>(mParams, fn);

		case IGNORE:
		default:
			return PromisePtr(PromiseIgnore());
		}
	}

	/*********************************************************
	 * If ParamType is not thenable, this promise can be
	 * implicitly converted into the ParamType.
	 **********************************************************/
	template <class Fn,
			 class T = ParamType,
			 ENABLE_IF(std::is_same<T, ParamType>),
			 ENABLE_IF(!IS_THENABLE(T))>
	inline T Convert(void) const
	{
		switch (mPromiseType) {
		case SUCCEED:
			return mParams;

		case IGNORE:
		default:
			throw ES("Some promise error is not handled");
		}
	}

private:
	ParamType mParams;
};

/* This is a SyncPromiseBase with ParamType and ErrorType.
 * Then method is used to handle ParamType.
 * Catch method is used to handle ErrorType. */
template <class P, class E>
class CSyncPromiseBase<P, E> :
	public CPromiseBase,
	public CThenable,
	public CCatchable
{
	typedef TO_PROMISABLE_TYPE(P) ParamType;
	typedef TO_PROMISABLE_TYPE(E) ErrorType;

public:
	/**********************************************************
	 * If the previous Catch method does not reocver the error,
	 * we provide a method to create a Ignore Promise.
	 * The Ignore promise will not call Then/Catch method and
	 * it will only keep creating other Ignore Promise.
	 **********************************************************/

	/* Both ParamType and ErrorType can be constructed by nullptr */
	template <class T1 = ParamType,
			 ENABLE_IF(std::is_same<T1, ParamType>),
			 class T2 = ErrorType,
			 ENABLE_IF(std::is_same<T2, ErrorType>),
			 ENABLE_IF(has_constructor<T1, std::nullptr_t>),
			 ENABLE_IF(has_constructor<T2, std::nullptr_t>)>
	inline CSyncPromiseBase(const PromiseIgnore &) :
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
	template <class T1 = ParamType,
			 ENABLE_IF(std::is_same<T1, ParamType>),
			 class T2 = ErrorType,
			 ENABLE_IF(std::is_same<T2, ErrorType>),
			 ENABLE_IF(!has_constructor<T1, std::nullptr_t>),
			 ENABLE_IF(has_constructor<T2, std::nullptr_t>)>
	inline CSyncPromiseBase(enum PromiseType = IGNORE) :
		CPromiseBase(IGNORE),
		mErrors(nullptr)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/* ParamType can be constructed by nullptr */
	template <class T1 = ParamType,
			 ENABLE_IF(std::is_same<T1, ParamType>),
			 class T2 = ErrorType,
			 ENABLE_IF(std::is_same<T2, ErrorType>),
			 ENABLE_IF(has_constructor<T1, std::nullptr_t>),
			 ENABLE_IF(!has_constructor<T2, std::nullptr_t>)>
	inline CSyncPromiseBase(enum PromiseType = IGNORE) :
		CPromiseBase(IGNORE),
		mParams(nullptr)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);
	}

	/* Neither ParamType nor ErrorType can be constructed by nullptr */
	template <class T1 = ParamType,
			 ENABLE_IF(std::is_same<T1, ParamType>),
			 class T2 = ErrorType,
			 ENABLE_IF(std::is_same<T2, ErrorType>),
			 ENABLE_IF(!has_constructor<T1, std::nullptr_t>),
			 ENABLE_IF(!has_constructor<T2, std::nullptr_t>)>
	inline CSyncPromiseBase(enum PromiseType = IGNORE) :
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
	template <class T1 = ParamType,
			 ENABLE_IF(std::is_same<T1, ParamType>),
			 class T2 = ErrorType,
			 ENABLE_IF(std::is_same<T2, ErrorType>),
			 ENABLE_IF(std::is_same<T1, T2>)>
	inline CSyncPromiseBase(const T1 &params,
							enum PromiseType type) :
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
	template <class T1 = ParamType,
			 ENABLE_IF(std::is_same<T1, ParamType>),
			 class T2 = ErrorType,
			 ENABLE_IF(std::is_same<T2, ErrorType>),
			 ENABLE_IF(!std::is_same<T1, T2>),
			 ENABLE_IF(has_constructor<T2, std::nullptr_t>)>
	inline CSyncPromiseBase(const T1 &params, enum PromiseType = SUCCEED) :
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
	template <class T1 = ParamType,
			 ENABLE_IF(std::is_same<T1, ParamType>),
			 class T2 = ErrorType,
			 ENABLE_IF(std::is_same<T2, ErrorType>),
			 ENABLE_IF(!std::is_same<T1, T2>),
			 ENABLE_IF(!has_constructor<T2, std::nullptr_t>)>
	inline CSyncPromiseBase(const T1 &params, enum PromiseType = SUCCEED) :
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
	template <class T1 = ParamType,
			 ENABLE_IF(std::is_same<T1, ParamType>),
			 class T2 = ErrorType,
			 ENABLE_IF(std::is_same<T2, ErrorType>),
			 ENABLE_IF(!std::is_same<T1, T2>),
			 ENABLE_IF(has_constructor<T1, std::nullptr_t>)>
	inline CSyncPromiseBase(const T2 &errors, enum PromiseType = FAIL) :
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
	template <class T1 = ParamType,
			 ENABLE_IF(std::is_same<T1, ParamType>),
			 class T2 = ErrorType,
			 ENABLE_IF(std::is_same<T2, ErrorType>),
			 ENABLE_IF(!std::is_same<T1, T2>),
			 ENABLE_IF(!has_constructor<T1, std::nullptr_t>)>
	inline CSyncPromiseBase(const T2 &errors, enum PromiseType = FAIL) :
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
			 class T = ParamType,
			 ENABLE_IF(std::is_same<T, ParamType>),
			 ENABLE_IF(IS_THENABLE(T))>
	inline decltype(auto) Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Then",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);

		typedef FN_RET_TYPE(Fn) RetType;
		typedef CSharedPtr<CSyncPromiseBase<RetType, ErrorType>> PromisePtr;

		switch (mPromiseType) {
		case SUCCEED:
			return ThenCreatePromise<PromisePtr>(mParams, fn);

		case FAIL:
			return PromisePtr(mErrors, FAIL);

		case IGNORE:
		default:
			return PromisePtr(PromiseIgnore());
		}
	}

	/*********************************************************
	 * If ParamType is not thenable, this promise can be
	 * implicitly converted into the ParamType.
	 **********************************************************/
	template <class Fn,
			 class T = ParamType,
			 ENABLE_IF(std::is_same<T, ParamType>),
			 ENABLE_IF(!IS_THENABLE(T))>
	inline T Convert(void) const
	{
		switch (mPromiseType) {
		case SUCCEED:
			return mParams;

		default:
			throw ES("Some promise error is not handled");
		}
	}

	/*********************************************************
	 * If ErrorType is catchable, Catch method is provided.
	 **********************************************************/

	/* Remove the ErrorType and let Catch method handle it */
	template <class Fn,
			 class T = ErrorType,
			 ENABLE_IF(std::is_same<T, ErrorType>),
			 ENABLE_IF(IS_CATCHABLE(T))>
	inline decltype(auto) Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Catch",
					  TYPE_NAME(ParamType),
					  TYPE_NAME(ErrorType),
					  mPromiseType);

		/* <ParamType, ErrorType> => <ParamType> */
		typedef CSharedPtr<CSyncPromiseBase<ParamType>> PromisePtr;

		switch (mPromiseType) {
		case SUCCEED:
			return PromisePtr(mParams, SUCCEED);

		case FAIL:
			return CatchCreatePromise<PromisePtr>(mErrors, fn);

		case IGNORE:
		default:
			return PromisePtr(PromiseIgnore());
		}
	}

private:
	ParamType mParams;
	ErrorType mErrors;
};

#endif /* __SYNC_PROMISE_BASE_HPP__ */

