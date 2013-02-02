/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 
#ifndef CASTS_HPP
#define CASTS_HPP

#include <cassert>
#include <string>
#include <sstream>

#include "BaseUid.hpp"
#include "Types.hpp"

template <class Derived, class Base>
inline Derived polymorphic_downcast(Base* x)
{
#ifndef NDEBUG  // add one more "if" for Android support plz
    assert(dynamic_cast<Derived>(x) != 0);
#endif
    return static_cast<Derived>(x);
}

template<typename T>
inline std::string cast_to_string(const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

template<>
inline std::string cast_to_string(const iviLink::BaseUid& u)
{
    return u.value();
}

template<>
inline std::string cast_to_string(const unsigned char& ch)
{
    return cast_to_string(unsigned(ch));
}

template<>
inline std::string cast_to_string(const signed char& ch)
{
    return cast_to_string(int(ch));
}

#endif //CASTS_HPP

