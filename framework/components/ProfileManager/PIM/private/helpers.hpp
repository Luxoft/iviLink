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


#ifndef PMPPIMHELPERS_HPP_
#define PMPPIMHELPERS_HPP_

#include <tr1/tuple>

namespace iviLink
{
namespace PMP
{
namespace PIM
{


/**
* Predicate for searching in containers of pairs by pair value
*/
template<typename Pair>
struct PairValueEquealPred
    : public std::unary_function<Pair, bool>
{
    /// Pair value type
    typedef typename Pair::second_type tValue;
    tValue const& value;

    /**
    * Constructor
    * @param value for search
    */
    PairValueEquealPred(tValue const& value)
        : value(value)
    {
    }

    bool operator() (Pair const& pair)
    {
        return pair.second == value;
    }
};


/**
* Predicate for searching by value of field in tuple
* Tuple - type of tuple
* field - field index
* Cmpr - type of field in tuple
*/
template <typename Tuple, size_t field, typename Cmpr>
class FindInTuplePred : public std::unary_function<Tuple, bool>
{
public:
    Cmpr const& cmpObj;
    FindInTuplePred(Cmpr const& cmpObj) : cmpObj(cmpObj)
    {
    }

    bool operator() (Tuple const& tuple)
    {
        return cmpObj == std::tr1::get<field>(tuple);
    }
};


}  // namespace PIM
}  // namespace PMP
}  // namespace AXIS


#endif /* HELPERS_HPP_ */
