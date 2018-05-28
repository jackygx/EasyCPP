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

#ifndef __SHARED_TOKEN_DECLARE_HPP__
#define __SHARED_TOKEN_DECLARE_HPP__

#include <Common/Common.hpp>
#include "SharedBase.hpp"

template <class T>
class CSharedPtr;

typedef OpsEnum TokenOps;

/* CSharedToken<T> can be used as the pdata in C codes.
 * For example: void SetCallbackFn(SomeFn fn, void *pdata);
 *
 * The CSharedToken<T> can be used to construct CSharedPtr<T>:
 * 1. Moved to a CSharedPtr<T>.
 *    After being moved, the CSharedToken<T> cannot be used again.
 *    CSharedPtr<T> t(CSharedToken<T> *token, MOVE);
 *
 * 2. Copied to multiple CSharedPtr<T>.
 *    After being copied, the CSharedToken<T> can be used again.
 *    CSharedPtr<T> t(CSharedToken<T> *token, COPY);
 *
 * After being moved or copied, the CSharedToken<T> must be manually released:
 *    token->Release();
 *    token = nullptr;
 */
template <class T>
class CSharedToken
{
	friend class CSharedPtr<T>;
private:
	inline CSharedToken(CSharedBase<T> *base);

public:
	inline void Release(void);

private:
	DEFINE_POOL_BASE(Pool, sizeof(CSharedPtr<T>));

	CSharedPtr<T> mPtr;
};

#else /* __SHARED_TOKEN_DECLARE_HPP__ */
#ifndef __SHARED_TOKEN_IMPLEMENT_HPP__
#define __SHARED_TOKEN_IMPLEMENT_HPP__

template <class T>
inline CSharedToken<T>::CSharedToken(CSharedBase<T> *base) :
	mPtr(base->AddRef())
{
	/* Does nothing */
}

template <class T>
inline void CSharedToken<T>::Release(void)
{
	SPTR_DEBUG("++[CSharedPtr<%s>(%p)]: token release", TYPE_NAME(T), this);

	mPtr.Release();
	Pool::Release((char *)this);

	SPTR_DEBUG("--[CSharedPtr<%s>(%p)]: token release", TYPE_NAME(T), this);
}

#endif /* __SHARED_TOKEN_IMPLEMENT_HPP__ */
#endif /* __SHARED_TOKEN_DECLARE_HPP__ */

