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

#ifndef __OBSERVER_HPP__
#define __OBSERVER_HPP__

#include <SharedPtr/SharedPtr.hpp>

template <class T, class... Tn>
class CListener
{
public:
	CListener(void) {}
	virtual ~CListener(void) {}

	virtual T OnNotify(Tn... params) = 0;
};

template <class T, class... Tn>
class CObserver
{
public:
	CObserver(void) :
		mListener(nullptr)
   	{
		/* Does nothing */
	}
	virtual ~CObserver(void)
	{
		/* Does nothing */
	}

	void RegisterListener(const CSharedPtr<CListener<T, Tn...>> &listener)
	{
		mListener = listener;
	}

protected:
	T NotifyListener(Tn... params)
	{
		if (mListener) {
			return mListener->OnNotify(std::move(params...));
		} else {
			throw E("Listener is not found");
		}
	}

protected:
	CSharedPtr<CListener<T, Tn...>> mListener;
};

template <class... Tn>
class CListener<void, Tn...>
{
public:
	CListener(void) {}
	virtual ~CListener(void) {}

	virtual void OnNotify(Tn... params) = 0;
};

template <class... Tn>
class CObserver<void, Tn...>
{
public:
	CObserver(void) :
		mListener(nullptr)
   	{
		/* Does nothing */
	}
	virtual ~CObserver(void)
	{
		/* Does nothing */
	}

	void RegisterListener(const CSharedPtr<CListener<void, Tn...>> &listener)
	{
		mListener = listener;
	}

protected:
	void NotifyListener(Tn... params)
	{
		if (mListener) {
			mListener->OnNotify(std::move(params...));
		}
	}

protected:
	CSharedPtr<CListener<void, Tn...>> mListener;
};

template <>
class CListener<void, void>
{
public:
	CListener(void) {}
	virtual ~CListener(void) {}

	virtual void OnNotify(void) = 0;
};

template <>
class CObserver<void, void>
{
public:
	CObserver(void) :
		mListener(nullptr)
   	{
		/* Does nothing */
	}
	virtual ~CObserver(void)
	{
		/* Does nothing */
	}

	void RegisterListener(const CSharedPtr<CListener<void, void>> &listener)
	{
		mListener = listener;
	}

protected:
	void NotifyListener(void)
	{
		if (mListener) {
			mListener->OnNotify();
		}
	}

protected:
	CSharedPtr<CListener<void, void>> mListener;
};

#define DEFINE_OBSERVER(name, ...) \
	typedef CListener<__VA_ARGS__> I##name##Listener; \
	typedef CSharedPtr<I##name##Listener> I##name##ListenerPtr; \
	typedef CObserver<__VA_ARGS__> I##name##Observer; \
	typedef CSharedPtr<I##name##Observer> I##name##ObserverPtr

#endif /* __OBSERVER_HPP__ */

