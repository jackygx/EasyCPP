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

#ifndef __SYNC_PROMISE_HEADER_HPP__
#define __SYNC_PROMISE_HEADER_HPP__

#include "PromiseParams.hpp"

/* Prototype of the CSyncPromise */
template <class... Tn>
class CSyncPromise {};

/* This struct is used to specify a situation
 * that the previous promise has failed and
 * the Catch function does not recover.
 * Then the new promise will be ignore.
 * Neither Then nor Catch method will be called. */
class CPromiseFail
{
public:
	CPromiseFail(void)
	{
		/* Does nothing */
	}
};

template <class... T>
struct CatchToPromise {};

/* If Catch does not return,
 * It does not recover the error. */
template <>
struct CatchToPromise<void>
{
	template <class PromisePtr,
			 class ErrorType,
			 class Fn>
	static inline PromisePtr Convert(ErrorType &&t, Fn &&fn)
	{
		t->Catch(fn);
		return PromisePtr(CPromiseFail());
	}
};

/* If Catch returns nullptr,
 * It does not recover the error. */
template <>
struct CatchToPromise<std::nullptr_t>
{
	template <class PromisePtr,
			 class ErrorType,
			 class Fn>
	static inline PromisePtr Convert(ErrorType &&t, Fn &&fn)
	{
		t->Catch(fn);
		return PromisePtr(CPromiseFail());
	}
};

/* If Catch returns PromisePtr,
 * It recovers the error. */
template <class T>
struct CatchToPromise<T>
{
	template <class PromisePtr,
			 class ErrorType,
			 class Fn>
	static inline PromisePtr Convert(ErrorType &&t, Fn &&fn)
	{
		return PromisePtr(t->Catch(fn));
	}
};

#define CATCH_TO_PROMISE(PromisePtr, mError, fn) \
	using RetType = decltype(mErrors->Catch(fn)); \
	using CConverter = CatchToPromise<RetType>; \
	return CConverter::template Convert<PromisePtr>(mErrors, fn);

template <class... T>
struct _ThenToPromise {};

/* If Catch does not return,
 * It does not recover the error. */
template <>
struct _ThenToPromise<void>
{
	template <class PromisePtr,
			 class PromiseType,
			 class Fn,
			 ENABLE_IF(has_constructor<PromisePtr, bool>)>
	static inline PromisePtr Convert(PromiseType &&t, Fn &&fn)
	{
		t->Then(fn);
		return PromisePtr(true);
	}

	template <class PromisePtr,
			 class PromiseType,
			 class Fn,
			 ENABLE_IF(!has_constructor<PromisePtr, bool>)>
	static inline PromisePtr Convert(PromiseType &&t, Fn &&fn)
	{
		t->Then(fn);
		return PromisePtr();
	}
};

/* If Catch returns PromisePtr,
 * It recovers the error. */
template <class T>
struct _ThenToPromise<T>
{
	template <class PromisePtr,
			 class PromiseType,
			 class Fn,
			 class RetType = T,
			 ENABLE_IF(has_constructor<PromisePtr, RetType, bool>)>
	static inline PromisePtr Convert(PromiseType &&t, Fn &&fn)
	{
		return PromisePtr(t->Then(fn), true);
	}

	template <class PromisePtr,
			 class PromiseType,
			 class Fn,
			 class RetType = T,
			 ENABLE_IF(!has_constructor<PromisePtr, RetType, bool>)>
	static inline PromisePtr Convert(PromiseType &&t, Fn &&fn)
	{
		return PromisePtr(t->Then(fn));
	}
};

template <class PromisePtr,
		 class ParamType,
		 class Fn,
		 ENABLE_IF(IS_PROMISABLE(ParamType))>
inline PromisePtr ThenToPromise(ParamType &&params, Fn &&fn)
{
	using RetType = decltype(params->Then(fn));
	using CConverter = _ThenToPromise<RetType>;

	return CConverter::template Convert<PromisePtr>(params, fn);
}

template <class PromisePtr,
		 class ParamType,
		 class Fn,
		 ENABLE_IF(!IS_PROMISABLE(ParamType))>
inline PromisePtr ThenToPromise(ParamType &&params, Fn &&fn)
{
	using RetType = decltype(params->Then(fn));
	using CConverter = _ThenToPromise<RetType>;

	return CConverter::template Convert<PromisePtr>(params, fn);
}

#endif /* __SYNC_PROMISE_HEADER_HPP__ */

