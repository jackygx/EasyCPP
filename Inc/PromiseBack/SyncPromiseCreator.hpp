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

#ifndef __SYNC_PROMISE_CREATOR_HPP__
#define __SYNC_PROMISE_CREATOR_HPP__

#include "PromiseParams.hpp"

/* Prorotype of the CCreateSyncPromise */
template <class... T>
class CCreateSyncPromise {};

/* Input type is a void */
template <>
struct CCreateSyncPromise<void>
{
	template <class PromisePtr,
			 class PromiseType,
			 class Fn>
	static inline PromisePtr Then(PromiseType &&t, Fn &&fn)
	{
		t->Then(fn);
		return PromisePtr(CPromiseBase::SUCCEED);
	}

	template <class PromisePtr,
			 class PromiseType,
			 class Fn>
	static inline PromisePtr Catch(PromiseType &&t, Fn &&fn)
	{
		t->Catch(fn);
		return PromisePtr(CPromiseBase::PromiseIgnore());
	}
};

/* Input type is a std::nullptr_t */
template <>
struct CCreateSyncPromise<std::nullptr_t>
{
	template <class PromisePtr,
			 class PromiseType,
			 class Fn>
	static inline PromisePtr Then(PromiseType &&t, Fn &&fn)
	{
		t->Then(fn);
		return PromisePtr(CPromiseBase::SUCCEED);
	}

	template <class PromisePtr,
			 class PromiseType,
			 class Fn>
	static inline PromisePtr Catch(PromiseType &&t, Fn &&fn)
	{
		t->Catch(fn);
		return PromisePtr(CPromiseBase::PromiseIgnore());
	}
};

/* Input type is a PromisePtr */
template <class T>
struct CCreateSyncPromise<T>
{
	template <class PromisePtr,
			 class PromiseType,
			 class Fn>
	static inline PromisePtr Then(PromiseType &&t, Fn &&fn)
	{
		return PromisePtr(t->Then(fn), CPromiseBase::SUCCEED);
	}

	template <class PromisePtr,
			 class PromiseType,
			 class Fn>
	static inline PromisePtr Catch(PromiseType &&t, Fn &&fn)
	{
		return PromisePtr(t->Then(fn), CPromiseBase::SUCCEED);
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
		 class ParamType,
		 class Fn,
		 ENABLE_IF(IS_CATCHABLE(ParamType))>
inline PromisePtr CatchCreatePromise(ParamType &&params, Fn &&fn)
{
	using RetType = decltype(params->Then(fn));
	using Creator = CCreateSyncPromise<RetType>;

	return Creator::template Catch<PromisePtr>(params, fn);
}

#endif /* __SYNC_PROMISE_CREATOR_HPP__ */

