/* 
 * 
 * iviLINK SDK, version 1.0
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 * 
 */



#include "samples/linux/AuthenticationApplication/CPINCode.hpp"

namespace authentication
{

bool operator==(const CPINCode& left, const CPINCode& right)
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

CPINCode::CPINCode()
   : isFirstDigitSet(false)
   , isSecondDigitSet(false)
   , isThirdDigitSet(false)
   , isFourthDigitSet(false)
{}

CPINCode::~CPINCode()
{

}

void CPINCode::setPIN(int firstDigit, int secondDigit, int thirdDigit, int fourthDigit)
{
   isFourthDigitSet = true;

   this->firstDigit = firstDigit;
   this->secondDigit = secondDigit;
   this->thirdDigit = thirdDigit;
   this->fourthDigit = fourthDigit;
}

string CPINCode::getEnteredDigitsAsString()
{
   string enteredDigits = string();

   if(isFourthDigitSet)
   {
      return digitToChar(firstDigit)
           + digitToChar(secondDigit)
           + digitToChar(thirdDigit)
           + digitToChar(fourthDigit);
   }
   else if(isThirdDigitSet)
   {
      return digitToChar(firstDigit)
           + digitToChar(secondDigit)
           + digitToChar(thirdDigit);
   }
   else if(isSecondDigitSet)
   {
      return digitToChar(firstDigit)
           + digitToChar(secondDigit);
   }
   else if(isFirstDigitSet)
   {
      return digitToChar(firstDigit);
   }
   else
   {
      return "";
   }
}

void CPINCode::pushToPIN(int digit)
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

void CPINCode::popFromPIN()
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

void CPINCode::clear()
{
    for (int i = 0 ; i < 4 /* digit count */ ; i++)
    {
        popFromPIN();
    }
}

bool CPINCode::isPINSet() const
{
   return isFourthDigitSet;
}

string CPINCode::digitToChar(int digit)
{
   switch(digit)
   {
   case 0:
      return "0";
   case 1:
      return "1";
   case 2:
      return "2";
   case 3:
      return "3";
   case 4:
      return "4";
   case 5:
      return "5";
   case 6:
      return "6";
   case 7:
      return "7";
   case 8:
      return "8";
   case 9:
      return "9";
   default:
      return "";
   }
}

} /* namespace authentication */
