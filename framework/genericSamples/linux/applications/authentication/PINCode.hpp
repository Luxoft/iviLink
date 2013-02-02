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


#ifndef PINCODE_HPP
#define PINCODE_HPP

#include <string>

namespace iviLink
{
namespace Authentication
{

class PINCode
{
public:
   PINCode();
   virtual ~PINCode();

   void setPIN(const int firstDigit, const int secondDigit, const int thirdDigit, const int fourthDigit);
   std::string getEnteredDigitsAsString() const;
   bool isPINSet() const;
   bool isPINEmpty() const;

   void pushToPIN(const int digit);
   void popFromPIN();
   void clear();

   int getFirstDigit() const
   {
      return firstDigit;
   }
   int getSecondDigit() const
   {
      return secondDigit;
   }
   int getThirdDigit() const
   {
      return thirdDigit;
   }
   int getFourthDigit() const
   {
      return fourthDigit;
   }

   friend bool operator==(const PINCode& left, const PINCode& right);

private:

   std::string digitToString(const int digit) const;

   int firstDigit;
   int secondDigit;
   int thirdDigit;
   int fourthDigit;

   bool isFirstDigitSet;
   bool isSecondDigitSet;
   bool isThirdDigitSet;
   bool isFourthDigitSet;
};

} /* namespace authentication */
}

#endif /* PINCODE_HPP_ */
