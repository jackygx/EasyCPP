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

#include "SyncPromiseParams.hpp"

template <class... T>
class CSyncPromiseBase {};

/**********************************************************
 * <Then> Only SyncPromiseBase:
 *
 * 1. The SyncPromise is created without <CatchType>.
 * 2. The <CatchType> is handled by the <Catch>.
 **********************************************************/
template <class... Tn>
class CSyncPromiseBase<CSharedPtr<CSyncPromiseThenParams<Tn...>>> :
	public CPromiseBase,
	public CThenable
{
	typedef CSharedPtr<CSyncPromiseThenParams<Tn...>> ThenType;

public:
	/* If the previous Catch method does not reocver the error,
	 * we provide a method to create a Ignored Promise.
	 * The Ignored promise will not call <Then> or <Catch> method.
	 * It will only keep creating other Ignored Promise. */
	inline CSyncPromiseBase(const Ignore &) :
		CPromiseBase(IGNORE),
		mThenParams(nullptr)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s>[%d]: Constructor",
					  TYPE_NAME(ThenType), mPromiseType);
	}

	inline CSyncPromiseBase(const ThenType &params, CPromiseBase::Type type) :
		CPromiseBase(type),
		mThenParams(params)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s>[%d]: Constructor",
					  TYPE_NAME(ThenType), mPromiseType);
	}

	/* <Then> method:
	 * Return type: RetType = CSharedPtr<CSyncPromiseBase<FnRetType>>
	 * For succeed: Init RetType with FnRetType.
	 * For Fail:    <Then> method should not be called. See below comments.
	 * For Ignore:  Init RetTYpe with CPromiseBase::IGNORE. */
	template <class Fn,
			 TYPE_ALIAS(_ThenType, ThenType),
			 TYPE_ALIAS(FnRetType, FN_RET_TYPE(Fn)),
			 TYPE_ALIAS(RetType, REMOVE_REFERENCE(decltype(std::declval<ThenType>()->Then(std::declval<Fn>()))))>
	inline RetType Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s>[%d]: Then",
					  TYPE_NAME(ThenType), mPromiseType);

		switch (mPromiseType) {
		case SUCCEED:
			return mThenParams->Then(fn);

		/* 1. The SyncPromise is created without <CatchType>.
		 *    The mPromiseType should always be SUCCEED.
		 *
		 * 2. The <CatchType> is handled by the <Catch>.
		 *    If the mPromiseType is FAIL, the <Then> method should not be called.
		 */
		case FAIL:
			throw ES("<Then> method should never be called for a failed <Then-only> SyncPromise");

		case IGNORE:
		default:
			return RetType(CPromiseBase::Ignore());
		}
	}

public:
	/* Debug method:
	 *
	 * Return type: true or false.
	 *
	 * Check if the type matches */
	template <class NewThenType,
			 TYPE_ALIAS(_ThenType, ThenType),
			 ENABLE_IF(IS_SAME_TYPE(NewThenType, _ThenType))>
	inline decltype(auto) CheckType(CPromiseBase::Type type, const char *func, int line)
	{
		if (mPromiseType != type) {
			PROMISE_ERROR("%s[%d]\nInput base type: %s\n Real base type: %s",
						  func, line, sTypeName[type], sTypeName[mPromiseType]);
		    throw ES("SyncPromise Check fail");
		}

		return this;
	}

	template <class NewThenType,
			 TYPE_ALIAS(_ThenType, ThenType),
			 ENABLE_IF(!IS_SAME_TYPE(NewThenType, _ThenType))>
	inline decltype(auto) CheckType(CPromiseBase::Type, const char *func, int line)
	{
		PROMISE_ERROR("%s[%d]\nInput type: %s\n Real type: %s",
					  func, line, TYPE_NAME(NewThenType), TYPE_NAME(ThenType));
		throw ES("SyncPromise Check fail");
		return this;
	}

private:
	ThenType mThenParams;
};


/**********************************************************
 * <Catch> Only SyncPromiseBase:
 *
 * 1. When the <Then> method of the previous SyncPromise
 *    returns <void>.
 **********************************************************/
template <class... Cn>
class CSyncPromiseBase<CSharedPtr<CSyncPromiseCatchParams<Cn...>>> :
	public CPromiseBase,
	public CCatchable
{
	typedef CSharedPtr<CSyncPromiseCatchParams<Cn...>> CatchType;

public:
	/* If the previous Catch method does not reocver the error,
	 * we provide a method to create a Ignored Promise.
	 * The Ignored promise will not call <Then> or <Catch> method.
	 * It will only keep creating other Ignored Promise. */
	inline CSyncPromiseBase(const Ignore &) :
		CPromiseBase(IGNORE),
		mCatchParams(nullptr)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s>[%d]: Constructor",
					  TYPE_NAME(CatchType), mPromiseType);
	}

	inline CSyncPromiseBase(const CatchType &params, CPromiseBase::Type type) :
		CPromiseBase(type),
		mCatchParams(params)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s>[%d]: Constructor",
					  TYPE_NAME(CatchType), mPromiseType);
	}

public:
	/* <Catch> method:
	 *
	 * Return type: Nothing to return.
	 *
	 * For succeed: <Catch> should never be called if the promis is SUCCESS.
	 * For Fail:    Init CatchRetType with FnRetType.
	 * For Ignore:  Does nothing. */
	template <class Fn>
	inline void Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s>[%d]: Catch",
					  TYPE_NAME(CatchType), mPromiseType);

		switch (mPromiseType) {
		case SUCCEED:
			throw ES("<Catch> method should never be called for a succeed <Catch-only> SyncPromise");

		case FAIL:
			mCatchParams->Catch(fn);

		case IGNORE:
		default:
			return;
		}
	}

public:
	/* Debug method:
	 *
	 * Return type: true or false.
	 *
	 * Check if the type matches */
	template <class NewCatchType,
			 TYPE_ALIAS(_CatchType, CatchType),
			 ENABLE_IF(IS_SAME_TYPE(NewCatchType, _CatchType))>
	inline decltype(auto) CheckType(CPromiseBase::Type type, const char *func, int line)
	{
		if (mPromiseType != type) {
			PROMISE_ERROR("%s[%d]\nInput base type: %s\n Real base type: %s",
						  func, line, sTypeName[type], sTypeName[mPromiseType]);
		    throw ES("SyncPromise Check fail");
		}

		return this;
	}

	template <class NewCatchType,
			 TYPE_ALIAS(_CatchType, CatchType),
			 ENABLE_IF(!IS_SAME_TYPE(NewCatchType, _CatchType))>
	inline decltype(auto) CheckType(CPromiseBase::Type, const char *func, int line)
	{
		PROMISE_ERROR("%s[%d]\nInput type: %s\n Real type: %s",
					  func, line, TYPE_NAME(NewCatchType), TYPE_NAME(CatchType));
		throw ES("SyncPromise Check fail");
		return this;
	}

protected:
	CatchType mCatchParams;
};


/**********************************************************
 * A SyncPromiseBase has both ThenType and CatchType.
 *
 * ThenType and CatchType are different type.
 *
 * <Then>  method is used to handle ThenType.
 * <Catch> method is used to handle CatchType.
 **********************************************************/
template <class... Tn, class... Cn>
class CSyncPromiseBase<CSharedPtr<CSyncPromiseThenParams<Tn...>>,
					   CSharedPtr<CSyncPromiseCatchParams<Cn...>>> :
	public CPromiseBase,
	public CThenable,
	public CCatchable
{
	typedef CSharedPtr<CSyncPromiseThenParams<Tn...>> ThenType;
	typedef CSharedPtr<CSyncPromiseCatchParams<Cn...>> CatchType;

public:
	inline CSyncPromiseBase(const Ignore &) :
		CPromiseBase(IGNORE),
		mThenParams(nullptr),
		mCatchParams(nullptr)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ThenType), TYPE_NAME(CatchType), mPromiseType);
	}

	inline CSyncPromiseBase(const ThenType &params, CPromiseBase::Type = SUCCEED) :
		CPromiseBase(SUCCEED),
		mThenParams(params),
		mCatchParams(nullptr)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ThenType), TYPE_NAME(CatchType), mPromiseType);
	}

	inline CSyncPromiseBase(const CatchType &params, CPromiseBase::Type = FAIL) :
		CPromiseBase(FAIL),
		mThenParams(nullptr),
		mCatchParams(params)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ThenType), TYPE_NAME(CatchType), mPromiseType);
	}

	inline CSyncPromiseBase(CPromiseBase::Type type) :
		CPromiseBase(type),
		mThenParams(nullptr),
		mCatchParams(nullptr)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Constructor",
					  TYPE_NAME(ThenType), TYPE_NAME(CatchType), mPromiseType);
	}

	inline void SetThenParams(ThenType &&params)
	{
		mThenParams = params;
	}

	inline void SetCatchParams(CatchType &&params)
	{
		mCatchParams = params;
	}

private:
	/* If the mThenParams->Then() does not return, the promise chain has finished. */
	template <class ThenRetType,
			 class Fn,
			 ENABLE_IF(IS_VOID(ThenRetType)),
			 TYPE_ALIAS(RetType, CSharedPtr<CSyncPromiseBase<CatchType>>)>
	inline RetType ThenSucceed(const Fn &fn)
	{
			mThenParams->Then(fn);
			return RetType(CPromiseBase::Ignore());
	}

	/* If the mThenParams->Then() returns a CSyncPromiseThenParams,
	 * returns CSharedPtr<CSyncPromiseBase<ThenRetType, CatchType>> */
	template <class ThenRetType,
			 class Fn,
			 ENABLE_IF(!IS_VOID(ThenRetType)),
			 ENABLE_IF(IS_SYNC_PROMISE_THEN_PARAM(ThenRetType)),
			 TYPE_ALIAS(RetType, CSharedPtr<CSyncPromiseBase<ThenRetType, CatchType>>)>
	inline RetType ThenSucceed(const Fn &fn)
	{
		return RetType(mThenParams->Then(fn), SUCCEED);
	}

	/* If the mThenParams->Then() returns a other value.
	 * returns CSharedPtr<CSyncPromiseBase<
	 *     CSharedPtr<CSyncPromiseThenParams<ThenRetType>>, CatchType>
	 * >> */
	template <class ThenRetType,
			 class Fn,
			 ENABLE_IF(!IS_VOID(ThenRetType)),
			 ENABLE_IF(!IS_SYNC_PROMISE_THEN_PARAM(ThenRetType)),
			 TYPE_ALIAS(NewThenType, CSharedPtr<CSyncPromiseThenParams<ThenRetType>>),
			 TYPE_ALIAS(RetType, CSharedPtr<CSyncPromiseBase<NewThenType, CatchType>>)>
	inline RetType ThenSucceed(const Fn &fn)
	{
		return RetType(NewThenType(mThenParams->Then(fn)));
	}


public:
	/* <Then> method:
	 *
	 * Return type: If the Fn does not return,
	 *              returns the <catch-only> SyncPromise.
	 *
	 *				If the Fn returns a CSyncPromiseThenParams,
	 *              returns CSharedPtr<CSyncPromiseBase<FnRetType, CatchType>>
	 *
	 *				If the Fn returns a other value.
	 *              returns CSharedPtr<CSyncPromiseBase<
	 *					CSharedPtr<CSyncPromiseThenParams<FnRetType>>, CatchType>
	 *				>>
	 * For succeed: Init RetType with IGNORE because the <Catch> method
	 *              should not be called for a SUCCESS SyncPromise..
	 * For Fail:    Init RetType with mCatchParams.
	 * For Ignore:  Init RetTYpe with CPromiseBase::IGNORE. */
	template <class Fn>
	inline decltype(auto) Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Then",
					  TYPE_NAME(ThenType), TYPE_NAME(CatchType), mPromiseType);

		using ThenRetType = decltype(mThenParams->Then(fn));
		using RetType = decltype(ThenSucceed<ThenRetType>(fn));


		switch (mPromiseType) {
		case SUCCEED:
			return ThenSucceed<ThenRetType>(fn);

		case FAIL:
			return RetType(mCatchParams, FAIL);

		case IGNORE:
		default:
			return RetType(CPromiseBase::Ignore());
		}
	}

private:
	/* If mThenParams is a promise, return the promise. */
	template <TYPE_ALIAS(_ThenType, ThenType),
			 ENABLE_IF(IS_PARAM_PROMISE(_ThenType))>
	inline decltype(auto) CatchSucceed(void) const
	{
		return mThenParams->GetPromise();
	}

	/* If mThenParams is NOT a promise, construct the new promise. */
	template <TYPE_ALIAS(_ThenType, ThenType),
			 ENABLE_IF(!IS_PARAM_PROMISE(_ThenType)),
			 TYPE_ALIAS(RetType, CSharedPtr<CSyncPromiseBase<_ThenType>>)>
	inline RetType CatchSucceed(void) const
	{
		return RetType(mThenParams, SUCCEED);
	}

	/* If the Fn does not return, the Promise is not recovered. */
	template <class RetType,
			 class Fn,
			 TYPE_ALIAS(FnRetType, FN_RET_TYPE(Fn)),
			 ENABLE_IF(IS_VOID(FnRetType))>
	inline RetType CatchFail(const Fn &fn) const
	{
		mCatchParams->Catch(fn);
		return RetType(CPromiseBase::Ignore());
	}

	/* If the Fn does return, the Promise is recovered. */
	template <class RetType,
			 class Fn,
			 TYPE_ALIAS(FnRetType, FN_RET_TYPE(Fn)),
			 ENABLE_IF(!IS_VOID(FnRetType))>
	inline RetType CatchFail(const Fn &fn) const
	{
		return mCatchParams->Catch(fn);
	}

public:
	/* <Catch> method:
	 *
	 * Return type: If mThenParams is a promise, 
	 *              RetType is the type of that promise;
	 *
	 * For succeed: Init CatchRetType with mThenParams.
	 * For Fail:    Init CatchRetType with FnRetType.
	 * For Ignore:  Init CatchRetType with CPromiseBase::IGNORE. */
	template <class Fn,
			 TYPE_ALIAS(_ThenType, ThenType),
			 ENABLE_IF(IS_PARAM_PROMISE(_ThenType))>
	inline decltype(auto) Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromiseBase<%s, %s>[%d]: Catch",
					  TYPE_NAME(ThenType), TYPE_NAME(CatchType), mPromiseType);

		using RetType = decltype(CatchSucceed());

		switch (mPromiseType) {
		case SUCCEED:
			return CatchSucceed();

		case FAIL:
			return CatchFail<RetType>(fn);

		case IGNORE:
		default:
			return RetType(CPromiseBase::Ignore());
		}
	}

public:
	/* Debug method:
	 *
	 * Return type: true or false.
	 *
	 * Check if the type matches */
	template <class NewThenType,
			 class NewCatchType,
			 TYPE_ALIAS(_ThenType, ThenType),
			 TYPE_ALIAS(_CatchType, CatchType),
			 ENABLE_IF(IS_SAME_TYPE(NewThenType, _ThenType)),
			 ENABLE_IF(IS_SAME_TYPE(NewCatchType, _CatchType))>
	inline decltype(auto) CheckType(CPromiseBase::Type type, const char *func, int line)
	{
		if (mPromiseType != type) {
			PROMISE_ERROR("%s[%d]\nInput base type: %s\n Real base type: %s",
						  func, line, sTypeName[type], sTypeName[mPromiseType]);
		    throw ES("SyncPromise Check fail");
		}

		return this;
	}

	template <class NewThenType,
			 class NewCatchType,
			 TYPE_ALIAS(_ThenType, ThenType),
			 TYPE_ALIAS(_CatchType, CatchType),
			 ENABLE_IF(!IS_SAME_TYPE(NewThenType, _ThenType))>
	inline decltype(auto) CheckType(CPromiseBase::Type, const char *func, int line)
	{
		PROMISE_ERROR("%s[%d]\nInput type: %s\n          : %s\n"
					  " Real type: %s\n          : %s",
					  func, line,
					  TYPE_NAME(NewThenType), TYPE_NAME(NewCatchType),
					  TYPE_NAME(ThenType), TYPE_NAME(CatchType));
		throw ES("SyncPromise Check fail");
		return this;
	}

	template <class NewThenType,
			 class NewCatchType,
			 TYPE_ALIAS(_ThenType, ThenType),
			 TYPE_ALIAS(_CatchType, CatchType),
			 ENABLE_IF(IS_SAME_TYPE(NewThenType, _ThenType)),
			 ENABLE_IF(!IS_SAME_TYPE(NewCatchType, _CatchType))>
	inline decltype(auto) CheckType(CPromiseBase::Type, const char *func, int line)
	{
		PROMISE_ERROR("%s[%d]\nInput type: %s\n          : %s\n"
					  " Real type: %s\n          : %s",
					  func, line,
					  TYPE_NAME(NewThenType), TYPE_NAME(NewCatchType),
					  TYPE_NAME(ThenType), TYPE_NAME(CatchType));
		throw ES("SyncPromise Check fail");
		return this;
	}

protected:
	ThenType mThenParams;
	CatchType mCatchParams;
};

#define CHECK_TYPE2(Type, Status) CheckType<Type>(Status, __func__, __LINE__)
#define CHECK_TYPE3(ThenType, CatchType, Status) CheckType<ThenType, CatchType>(Status, __func__, __LINE__)

#define CHECK_TYPE(...) CONNECT(CHECK_TYPE, NARG(__VA_ARGS__))(__VA_ARGS__)

#endif /* __SYNC_PROMISE_BASE_HPP__ */

