/* 
 * 
 * iviLINK SDK, version 1.1.2
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







#ifndef CPINCODE_HPP
#define CPINCODE_HPP

#include <string>

using namespace std;

namespace authentication
{

class CPINCode
{
public:
   CPINCode();
   virtual ~CPINCode();

   void setPIN(int firstDigit, int secondDigit, int thirdDigit, int fourthDigit);
   string getEnteredDigitsAsString();
   bool isPINSet() const;

   void pushToPIN(int digit);
   void popFromPIN();
   void clear();

   int getFirstDigit(){return firstDigit;}
   int getSecondDigit(){return secondDigit;}
   int getThirdDigit(){return thirdDigit;}
   int getFourthDigit(){return fourthDigit;}

   friend bool operator==(const CPINCode& left, const CPINCode& right);

private:

   string digitToChar(int digit);

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

#endif /* CPINCODE_HPP_ */
