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

#ifndef __JSON_HPP__
#define __JSON_HPP__

#include <EasyCpp.hpp>
#include <Pool/MemPool.hpp>
#include <DataStruct/List.hpp>

#define JSON_BUF_SIZE 102400
typedef CMemPool<JSON_BUF_SIZE> JsonBufPool;

DEFINE_CLASS(Json);

DEFINE_FUNC(JsonNotify, void(const CJsonPtr &));
DEFINE_FUNC(JsonMiss, void(const CConstJsonPtr &));
DEFINE_SYNC_PROMISE(Json, (CJsonPtr), (void));

class CJson :
	public CEnableSharedPtr<CJson>
{
public:
	enum Type {
		OBJECT,
		ARRAY,
	};

public:
	CJson(const CConstStringPtr &key = nullptr,
		  const CConstStringPtr &val = nullptr,
		  CJson::Type type = OBJECT);
	virtual ~CJson(void);

	template <class Fn>
    inline CJsonPtr GetKey(const Fn &fn);
	template <class Fn>
    inline CConstJsonPtr GetKey(const Fn &fn) const;
    inline CConstStringPtr GetKey(void) const;

	template <class Fn>
    inline CJsonPtr GetVal(const Fn &fn);
	template <class Fn>
    inline CConstJsonPtr GetVal(const Fn &fn) const;
    inline CConstStringPtr GetVal(void) const;

	template <class Fn>
	inline CJsonPtr GetChild(const Fn &fn);
	template <class Fn>
	inline CConstJsonPtr GetChild(const Fn &fn) const;
	inline CJsonPtr GetChild(void) const;

	template <class Fn>
	inline CJsonPtr GetSibling(const Fn &fn);
	template <class Fn>
	inline CConstJsonPtr GetSibling(const Fn &fn) const;
	inline CJsonPtr GetSibling(void) const;

	inline decltype(auto) GetChildByKey(const CConstStringPtr &key);

	template <class Fn>
	inline CJsonPtr GetChildByKey(const CConstStringPtr &key, const Fn &fn);
	template <class Fn>
	inline CConstJsonPtr GetChildByKey(const CConstStringPtr &key, const Fn &fn) const;
	inline CJsonPtr GetChildByKey(const CConstStringPtr &key) const;

	template <class Fn>
	inline CJsonPtr GetChildByVal(const CConstStringPtr &val, const Fn &fn);
	template <class Fn>
	inline CConstJsonPtr GetChildByVal(const CConstStringPtr &val, const Fn &fn) const;
	inline CJsonPtr GetChildByVal(const CConstStringPtr &val) const;

	template <class Fn>
    inline CJsonPtr GetType(const Fn &fn);
	template <class Fn>
    inline CConstJsonPtr GetType(const Fn &fn) const;
	inline CJson::Type GetType(void) const;

	inline void SetKey(const CConstStringPtr &key);
	inline void SetVal(const CConstStringPtr &val);

	inline CJsonPtr AddChild(const CJsonPtr &child);
	inline CJsonPtr AddChild(const CConstStringPtr &key = nullptr,
							 const CConstStringPtr &val = nullptr,
							 CJson::Type type = OBJECT);
	DEFINE_FUNC(JsonCb, void(const CJsonPtr &));
	inline CJsonPtr AddChild(const CJsonPtr &child,
							 const JsonCbFn &cb);

	template <class... Tn, typename = JsonCbFn &&>
	inline CJsonPtr AddSibling(Tn... tn, JsonCbFn &&cb);
	inline CJsonPtr AddSibling(const CConstStringPtr &key = nullptr,
							   const CConstStringPtr &val = nullptr,
							   CJson::Type type = OBJECT);
	inline CJsonPtr AddSibling(const CJsonPtr &sibling);

	CConstStringPtr ToString(void) const;

	void Dump(uint8_t lev) const;

	template <class T1,
			 ENABLE_IF(IS_STATICALLY_ASSIGNABLE(T1, CConstStringPtr))>
	inline T1 Convert(void) const
	{
		return GetVal();
	}

	DEFINE_ITERATOR(Iterator, CJsonPtr);
	class Iterator :
		public IteratorBase
	{
		friend IteratorBase;

	public:
		enum MatchType {
			KEY,
			VAL,
		};
	
	public:
		inline Iterator(const CJsonPtr &parent);

	private:
		inline void _Begin(void);
		inline bool _End(void) const;
		inline void _Next(void);

		template <class Fn, class... Tn>
		inline decltype(auto) _Get(const Fn &fn, const Tn & ... tn);

		inline bool _Match(const CConstStringPtr &data,
						   CJson::Iterator::MatchType type = CJson::Iterator::KEY);

		inline void _Erase(void);
		inline void _Insert(const CJsonPtr &json);

		inline void _Sort(CJson::Iterator::MatchType type = CJson::Iterator::KEY);

		template <class Fn>
		void __Sort(const Fn &fn);

	private:
		CJsonPtr mParent;
		CJsonPtr mPrev;
		CJsonPtr mCurrent;
	};

	inline IteratorPtr GetChildren(void);

	friend class CJson::Iterator;
private:
	uint32_t GetSize(void) const;
	void _ToString(CStringPtr &str) const;

private:
	CJson::Type mType;
	CConstStringPtr mKey;
	CConstStringPtr mVal;

	CJsonPtr mChild;
	CJsonPtr mSibling;
};

template <class Fn>
inline CJsonPtr CJson::GetKey(const Fn &fn)
{
	fn(mKey);
	return Share();
}

template <class Fn>
inline CConstJsonPtr CJson::GetKey(const Fn &fn) const
{
	fn(mKey);
	return Share();
}

inline CConstStringPtr CJson::GetKey(void) const
{
	if (!mKey) {
		throw E("Empty key");
	}

	return mKey;
}

template <class Fn>
inline CJsonPtr CJson::GetVal(const Fn &fn)
{
	fn(mVal);
	return Share();
}

template <class Fn>
inline CConstJsonPtr CJson::GetVal(const Fn &fn) const
{
	fn(mVal);
	return Share();
}

inline CConstStringPtr CJson::GetVal(void) const
{
	if (!mVal) {
		throw E("Empty val");
	}

	return mVal;
}

template <class Fn>
inline CJsonPtr CJson::GetChild(const Fn &fn)
{
	fn(mChild);
	return Share();
}

template <class Fn>
inline CConstJsonPtr CJson::GetChild(const Fn &fn) const
{
	fn(mChild);
	return Share();
}

inline CJsonPtr CJson::GetChild(void) const
{
	if (!mChild) {
		throw E("No child is found");
	}

	return mChild;
}

template <class Fn>
inline CJsonPtr CJson::GetSibling(const Fn &fn)
{
	fn(mSibling);
	return Share();
}

template <class Fn>
inline CConstJsonPtr CJson::GetSibling(const Fn &fn) const
{
	fn(mSibling);
	return Share();
}

inline CJsonPtr CJson::GetSibling(void) const
{
	if (!mSibling) {
		throw E("No sibling is found");
	}

	return mSibling;
}

inline decltype(auto) CJson::GetChildByKey(const CConstStringPtr &key)
{
	for (auto it(mChild); it; it = it->mSibling) {
		if (it->mKey == key) {
			return CJsonPromisePtr(it);
		}
	}

	return CJsonPromisePtr();
}

template <class Fn>
inline CJsonPtr CJson::GetChildByKey(const CConstStringPtr &key, const Fn &fn)
{
	for (auto it(mChild); it; it = it->mSibling) {
		if (it->mKey == key) {
			fn(it);
		}
	}

	return Share();
}

template <class Fn>
inline CConstJsonPtr CJson::GetChildByKey(const CConstStringPtr &key, const Fn &fn) const
{
	for (auto it(mChild); it; it = it->mSibling) {
		if (it->mKey == key) {
			fn(it);
		}
	}

	return Share();
}

inline CJsonPtr CJson::GetChildByKey(const CConstStringPtr &key) const
{
	for (auto it(mChild); it; it = it->mSibling) {
		if (it->mKey == key) {
			return it;
		}
	}

	throw E("Cannot find child with key: ", key);
}

template <class Fn>
inline CJsonPtr CJson::GetChildByVal(const CConstStringPtr &val, const Fn &fn)
{
	for (auto it(mChild); it; it = it->mSibling) {
		if (it->mVal == val) {
			fn(it);
		}
	}

	return Share();
}

template <class Fn>
inline CConstJsonPtr CJson::GetChildByVal(const CConstStringPtr &val, const Fn &fn) const
{
	for (auto it(mChild); it; it = it->mSibling) {
		if (it->mVal == val) {
			fn(it);
		}
	}

	return Share();
}

inline CJsonPtr CJson::GetChildByVal(const CConstStringPtr &val) const
{
	for (auto it(mChild); it; it = it->mSibling) {
		if (it->mVal == val) {
			return it;
		}
	}

	throw E("Cannot find child with val: ", val);
}

template <class Fn>
inline CJsonPtr CJson::GetType(const Fn &fn)
{
	fn(mType);
	return Share();
}

template <class Fn>
inline CConstJsonPtr CJson::GetType(const Fn &fn) const
{
	fn(mType);
	return Share();
}

inline enum CJson::Type CJson::GetType(void) const
{
	return mType;
}

inline void CJson::SetKey(const CConstStringPtr &key)
{
	mKey = key;
}

inline void CJson::SetVal(const CConstStringPtr &val)
{
	mVal = val;
}

inline CJsonPtr CJson::AddChild(const CJsonPtr &child)
{
	if (mChild) {
		mChild->AddSibling(child);
	} else {
		mChild = child;
	}

	return Share();
}

inline CJsonPtr CJson::AddChild(const CConstStringPtr &key,
								const CConstStringPtr &val,
								enum CJson::Type type)
{
	return AddChild(CJsonPtr(key, val, type));
}

inline CJsonPtr CJson::AddChild(const CJsonPtr &child,
								const CJson::JsonCbFn &cb)
{
	if (mChild) {
		mChild->AddSibling(child);
	} else {
		mChild = child;
	}

	cb(child);

	return Share();
}

template <class... Tn, typename>
inline CJsonPtr CJson::AddSibling(Tn... tn, CJson::JsonCbFn &&cb)
{
	CJsonPtr sibling(tn...);

	if (mSibling) {
		mSibling->AddSibling(sibling);
	} else {
		mSibling = sibling;
	}

	cb(mChild);

	return Share();
}

inline CJsonPtr CJson::AddSibling(const CConstStringPtr &key,
								  const CConstStringPtr &val,
								  CJson::Type type)
{
	CJsonPtr sibling(key, val, type);

	if (mSibling) {
		mSibling->AddSibling(sibling);
	} else {
		mSibling = sibling;
	}

	return Share();
}

inline CJsonPtr CJson::AddSibling(const CJsonPtr &sibling)
{
	if (mSibling) {
		mSibling->AddSibling(sibling);
	} else {
		mSibling = sibling;
	}

	return Share();
}

inline CJson::Iterator::Iterator(const CJsonPtr &parent) :
	mParent(parent),
	mPrev(nullptr),
	mCurrent(mParent->mChild)
{
	/* Does nothing */
}

inline void CJson::Iterator::_Begin(void)
{
	mPrev = nullptr;
	mCurrent = mParent->mChild;
}

inline bool CJson::Iterator::_End(void) const
{
	return !mCurrent;
}

inline void CJson::Iterator::_Next(void)
{
	if (mCurrent) {
		mPrev = mCurrent;
		mCurrent = mCurrent->mSibling;
	}
}

template <class Fn, class... Tn>
inline decltype(auto) CJson::Iterator::_Get(const Fn &fn, const Tn & ... tn)
{
	return fn(mCurrent, tn...);
}

inline bool CJson::Iterator::_Match(const CConstStringPtr &data,
									CJson::Iterator::MatchType type)
{
	return data == ((CJson::Iterator::KEY == type) ? 
					mCurrent->mKey : mCurrent->mVal);
}

inline void CJson::Iterator::_Erase(void)
{
	mCurrent = mCurrent->mSibling;

	if (mPrev) {
		mPrev->mSibling = mCurrent;
	} else {
		mParent->mChild = mCurrent;
	}
}

inline void CJson::Iterator::_Insert(const CJsonPtr &json)
{
	json->mSibling = mCurrent;

	if (mPrev) {
		mPrev->mSibling = json;
	} else {
		mParent->mChild = json;
	}

	mPrev = json;
}

template <class Fn>
void CJson::Iterator::__Sort(const Fn &fn)
{
	auto head(mParent->mChild);
	/* Nothing to sort */
	if (!head) {
		return;
	}

	mParent->mChild = nullptr;

	auto next(head->mSibling);
	head->mSibling = nullptr;

	while (next) {
		auto tmp(next->mSibling);
		auto nkey(fn(next));

		/* No key */
		if (!nkey) {
			/* Insert the next to new head */
			next->mSibling = head;
			head = next;
		} else {
			CJsonPtr *it = &(head);

			while (true) {
				auto ikey(fn(*it));

				/* Insert nkey before (*it) */
				if (ikey && (ikey > nkey)) {
					next->mSibling = (*it);
					(*it) = next;
					break;
				}

				it = &((*it)->mSibling);

				/* To the end */
				if (!(*it)) {
					(*it) = next;
					next->mSibling = nullptr;
					break;
				}
			}
		}

		next.Swap(tmp);
	}

	mParent->mChild = head;
}

inline void CJson::Iterator::_Sort(CJson::Iterator::MatchType type)
{
	if (CJson::Iterator::KEY == type) {
		__Sort([](const CJsonPtr &json) {
			return json->mKey;
		});
	} else {
		__Sort([](const CJsonPtr &json) {
			return json->mVal;
		});
	}
}

inline CJson::IteratorPtr CJson::GetChildren(void)
{
	if (!mChild) {
		throw E("No child is found");
	}

	return CJson::IteratorPtr(Share());
}

#endif /* __JSON_HPP__ */

