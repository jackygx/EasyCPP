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

#ifndef __SYNC_PROMISE_HELP_HPP__
#define __SYNC_PROMISE_HELP_HPP__

#include <Common/Macro.hpp>

#include "SyncPromise.hpp"

#define PROMISE_PARAM_TYPE(...) \
	CSharedPtr<CPromiseParams<__VA_ARGS__>>

#define PROMISE_PACK_TYPE(...) \
	pack<__VA_ARGS__>

#define DEFINE_SYNC_PROMISE_1(name, params) \
	typedef PROMISE_PARAM_TYPE params C##name##PromiseParamsPtr; \
	typedef CSharedPtr<CSyncPromise<C##name##PromiseParamsPtr>> C##name##PromisePtr

#define DEFINE_SYNC_PROMISE_2(name, params, errors) \
	typedef PROMISE_PARAM_TYPE params C##name##PromiseParamsPtr; \
	typedef PROMISE_PARAM_TYPE errors C##name##PromiseErrorsPtr; \
	typedef PROMISE_PACK_TYPE params C##name##PromisePackParamsPtr; \
	typedef PROMISE_PACK_TYPE errors C##name##PromisePackErrorsPtr; \
	typedef CSharedPtr<CSyncPromise<C##name##PromisePackParamsPtr, \
									C##name##PromisePackErrorsPtr>> \
									C##name##PromisePtr

#define DEFINE_SYNC_PROMISE(name, ...) \
	CONNECT(DEFINE_SYNC_PROMISE_, NARG(__VA_ARGS__))(name, __VA_ARGS__)

template <class... T>
inline decltype(auto) CreateSyncPromise(T && ... t)
{
	typedef CSharedPtr<CPromiseParams<T...>> ParamType;
	typedef CSharedPtr<CSyncPromiseBase<ParamType>> PromisePtr;

	return PromisePtr(ParamType(t...));
}

#endif /* __SYNC_PROMISE_HELP_HPP__ */

