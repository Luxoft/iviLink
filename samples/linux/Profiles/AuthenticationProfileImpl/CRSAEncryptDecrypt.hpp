/* 
 * 
 * iviLINK SDK, version 1.0.1
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





#ifndef CRSAENCRYPTDECRYPT_HPP_
#define CRSAENCRYPTDECRYPT_HPP_

#include <string>
#include "utils/misc/Logger.hpp"
#include "3rd_party/cryptopp/cryptopp-5.6.1/rsa.h"
#include "3rd_party/cryptopp/cryptopp-5.6.1/integer.h"

using CryptoPP::Integer;
using CryptoPP::RSA;
using namespace std;

class CRSAEncryptDecrypt
{
public:
   CRSAEncryptDecrypt();
   virtual ~CRSAEncryptDecrypt();

public:

   static string encrypt(string stringToEncrypt, RSA::PublicKey publicKey);
   static string decrypt(string stringToDecrypt, RSA::PrivateKey privateKey);
   static RSA::PublicKey getPublicKey();
   static RSA::PrivateKey getPrivateKey();

   static void printPrivateKey(RSA::PrivateKey key)
   {
      ///////////////////////////////////////
      // Generated Parameters
      const Integer& n = key.GetModulus();
      const Integer& p = key.GetPrime1();
      const Integer& q = key.GetPrime2();
      const Integer& d = key.GetPrivateExponent();
      const Integer& e = key.GetPublicExponent();

      cout << "RSA Parameters:" << endl;
      cout << " n: " << n << endl;
      cout << " p: " << p << endl;
      cout << " q: " << q << endl;
      cout << " d: " << d << endl;
      cout << " e: " << e << endl;
      cout << endl;
   }

   static void printPublicKey(RSA::PublicKey key)
   {
      ///////////////////////////////////////
      // Generated Parameters
      const Integer& n = key.GetModulus();
      const Integer& e = key.GetPublicExponent();

      cout << "RSA Parameters:" << endl;
      cout << " n: " << n << endl;
      cout << " e: " << e << endl;
      cout << endl;

   }


private:

   static RSA::PrivateKey rsaPrivate;
   static bool privateKeyGenerated;
   //static RSA::PublicKey rsaPublic;

   static Logger msLogger;
};

#endif /* CRSAENCRYPTDECRYPT_HPP_ */
