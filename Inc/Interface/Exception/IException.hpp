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

#ifndef __IEXCEPTION_HPP__
#define __IEXCEPTION_HPP__

class IException
{
public:
	virtual void Show(void) const = 0;

	virtual ~IException(void) {}

public:
	inline static void SetSymbol(const char *path);
	inline static const char *GetSymbol(void);

private:
	static const char *sSymbolPath;
};

inline void IException::SetSymbol(const char *path)
{
	sSymbolPath = path;
}

inline const char *IException::GetSymbol(void)
{
	return sSymbolPath;
}

#define InitSymbolPath(path) \
	IException::SetSymbol(path)

#endif /* __IEXCEPTION_HPP__ */

