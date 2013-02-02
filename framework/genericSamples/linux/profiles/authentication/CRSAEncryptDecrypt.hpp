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


#ifndef CRSAENCRYPTDECRYPT_HPP_
#define CRSAENCRYPTDECRYPT_HPP_

#include <string>
#include <sstream>
#include <iostream>

#include "Logger.hpp"
#include "rsa.h"
#include "integer.h"

using CryptoPP::Integer;
using CryptoPP::RSA;

class CRSAEncryptDecrypt
{
public:
   CRSAEncryptDecrypt();
   virtual ~CRSAEncryptDecrypt();

public:

   static std::string encrypt(const std::string& stringToEncrypt, const RSA::PublicKey& publicKey);
   static std::string decrypt(const std::string& stringToDecrypt, const RSA::PrivateKey& privateKey);
   static RSA::PublicKey getPublicKey(const std::string& pathToKeyStorage);
   static RSA::PrivateKey getPrivateKey(const std::string& pathToKeyStorage);

private:
   static RSA::PrivateKey rsaPrivate;
   static bool privateKeyGenerated;
   static Logger msLogger;
};

#endif /* CRSAENCRYPTDECRYPT_HPP_ */
