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


#include "PINCode.hpp"

namespace iviLink
{
namespace Authentication
{

bool operator==(const PINCode& left, const PINCode& right)
{
    if(!left.isPINSet() || !right.isPINSet())
    {
        return false;
    }

    return left.firstDigit == right.firstDigit &&
           left.secondDigit == right.secondDigit &&
           left.thirdDigit == right.thirdDigit &&
           left.fourthDigit == right.fourthDigit;
}

PINCode::PINCode()
    : isFirstDigitSet(false)
    , isSecondDigitSet(false)
    , isThirdDigitSet(false)
    , isFourthDigitSet(false)
{}

PINCode::~PINCode()
{

}

void PINCode::setPIN(const int firstDigit,  const int secondDigit, const int thirdDigit, const int fourthDigit)
{
    isFourthDigitSet = true;
    isThirdDigitSet = true;
    isSecondDigitSet = true;
    isFirstDigitSet = true;

    this->firstDigit = firstDigit;
    this->secondDigit = secondDigit;
    this->thirdDigit = thirdDigit;
    this->fourthDigit = fourthDigit;
}

std::string PINCode::getEnteredDigitsAsString() const
{
    std::string enteredDigits = std::string();

    if(isFourthDigitSet)
    {
        return digitToString(firstDigit)
             + digitToString(secondDigit)
             + digitToString(thirdDigit)
             + digitToString(fourthDigit);
    }
    else if(isThirdDigitSet)
    {
        return digitToString(firstDigit)
             + digitToString(secondDigit)
             + digitToString(thirdDigit);
    }
    else if(isSecondDigitSet)
    {
        return digitToString(firstDigit)
             + digitToString(secondDigit);
    }
    else if(isFirstDigitSet)
    {
        return digitToString(firstDigit);
    }
    else
    {
        return "";
    }
}

void PINCode::pushToPIN(const int digit)
{
    if(!isFirstDigitSet)
    {
        firstDigit = digit;
        isFirstDigitSet = true;
    }
    else if(!isSecondDigitSet)
    {
        secondDigit = digit;
        isSecondDigitSet = true;
    }
    else if(!isThirdDigitSet)
    {
        thirdDigit = digit;
        isThirdDigitSet = true;
    }
    else if(!isFourthDigitSet)
    {
        fourthDigit = digit;
        isFourthDigitSet = true;
    }
}

void PINCode::popFromPIN()
{
    if(isFourthDigitSet)
    {
        isFourthDigitSet = false;
        return;
    }
    else if(isThirdDigitSet)
    {
        isThirdDigitSet = false;
        return;
    }
    else if(isSecondDigitSet)
    {
        isSecondDigitSet = false;
        return;
    }
    else if(isFirstDigitSet)
    {
        isFirstDigitSet = false;
        return;
    }
}

void PINCode::clear()
{
    isFourthDigitSet = false;
    isThirdDigitSet = false;
    isSecondDigitSet = false;
    isFirstDigitSet = false;
}

bool PINCode::isPINSet() const
{
    return isFourthDigitSet;
}

bool PINCode::isPINEmpty() const
{
    return getEnteredDigitsAsString() == std::string("");
}

std::string PINCode::digitToString(const int digit) const
{
    std::string result = "";
    if (digit < 0 || digit > 9)
    {
        return result;
    }
    char converted = (char)(((int)'0') + digit);
    result.push_back(converted);
    return result;
}

} /* namespace authentication */
}
