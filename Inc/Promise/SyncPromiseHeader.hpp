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

/* If Catch does not return,
 * It does not recover the error. */
template <class PromisePtr,
		 class RetType,
		 ENABLE_IF(std::is_same<RetType, void>)>
inline PromisePtr CatchRetToPromise(RetType)
{
	return PromisePtr(CPromiseFail());
}

/* If Catch returns nullptr,
 * It does not recover the error. */
template <class PromisePtr,
		 class RetType,
		 ENABLE_IF(std::is_same<RetType, std::nullptr_t>)>
inline PromisePtr CatchRetToPromise(RetType &&)
{
	return PromisePtr(CPromiseFail());
}

/* If Catch returns PromisePtr,
 * It recovers the error. */
template <class PromisePtr,
		 class RetType,
		 ENABLE_IF(!std::is_same<RetType, void>),
		 ENABLE_IF(!std::is_same<RetType, std::nullptr_t>)>
inline PromisePtr CatchRetToPromise(RetType && type)
{
	return PromisePtr(type);
}

#endif /* __SYNC_PROMISE_HEADER_HPP__ */

