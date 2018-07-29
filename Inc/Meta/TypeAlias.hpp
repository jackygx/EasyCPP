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

#ifndef __TYPE_ALISA_HPP__
#define __TYPE_ALISA_HPP__

/* The macro is used in the template.
 * The template of the class cannot be used
 * by its member function's meta program directly.
 * It can be fixed by using an alias of the type.
 * The macro also makes sure the caller cannot
 * explicitly define some other type of the alias.
 * Usage example:
 * template <TYPE_ALIAS(_T, T),
 *           ENABLE_IF(is_abstract<_T>)>
 * void Example(void);
 */
#define TYPE_ALIAS(Alias, T) \
	typename Alias = T, \
	ENABLE_IF(std::is_same<Alias, T>)

#endif /* __TYPE_ALISA_HPP__ */

