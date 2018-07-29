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
#include "PromiseBase.hpp"

/* Prorotype of the CCreateSyncPromise */
template <class... T>
class CCreateSyncPromise {};

/* Input type is a std::nullptr_t */
template <>
struct CCreateSyncPromise<std::nullptr_t>
{
	template <class PromisePtr,
			 class ParamType,
			 class Fn>
	static inline PromisePtr Then(ParamType &&params, Fn &&fn)
	{
		params->Then(fn);
		return PromisePtr(CPromiseBase::SUCCEED);
	}

	template <class PromisePtr,
			 class ErrorType,
			 class Fn>
	static inline PromisePtr Catch(ErrorType &&errors, Fn &&fn)
	{
		errors->Catch(fn);
		return PromisePtr(CPromiseBase::Ignore());
	}
};

/* Input type is a void */
template <>
struct CCreateSyncPromise<void>
{
	template <class PromisePtr,
			 class ParamType,
			 class Fn>
	static inline PromisePtr Then(ParamType &&params, Fn &&fn)
	{
		params->Then(fn);
		return PromisePtr(CPromiseBase::SUCCEED);
	}

	template <class PromisePtr,
			 class ErrorType,
			 class Fn>
	static inline PromisePtr Catch(ErrorType &&errors, Fn &&fn)
	{
		errors->Catch(fn);
		return PromisePtr(CPromiseBase::Ignore());
	}
};

/* Input type is a PromisePtr */
template <class T>
struct CCreateSyncPromise<T>
{
	template <class PromisePtr,
			 class ParamType,
			 class Fn,
			 ENABLE_IF(IS_THENABLE(ParamType))>
	static inline PromisePtr Then(ParamType &&params, Fn &&fn)
	{
		return PromisePtr(params->Then(fn), CPromiseBase::SUCCEED);
	}

	template <class PromisePtr,
			 class ErrorType,
			 class Fn,
			 ENABLE_IF(IS_CATCHABLE(ErrorType))>
	static inline PromisePtr Catch(ErrorType &&errors, Fn &&fn)
	{
		return errors->Catch(fn);
	}
};

template <class PromisePtr,
		 class ParamType,
		 class Fn,
		 ENABLE_IF(IS_THENABLE(ParamType))>
inline PromisePtr ThenCreatePromise(ParamType &&params, Fn &&fn)
{
	using RetType = decltype(params->Then(fn));
	using Creator = CCreateSyncPromise<RetType>;

	return Creator::template Then<PromisePtr>(params, fn);
}

template <class PromisePtr,
		 class ErrorType,
		 class Fn,
		 ENABLE_IF(IS_CATCHABLE(ErrorType))>
inline PromisePtr CatchCreatePromise(ErrorType &&errors, Fn &&fn)
{
	using RetType = decltype(errors->Catch(fn));
	using Creator = CCreateSyncPromise<RetType>;

	return Creator::template Catch<PromisePtr>(errors, fn);
}

#endif /* __SYNC_PROMISE_HEADER_HPP__ */

