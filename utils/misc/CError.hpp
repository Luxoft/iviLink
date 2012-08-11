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







#ifndef CERROR_HPP__
#define CERROR_HPP__

#include <cstring>
#include <string>
#include <sstream>

#include "Types.hpp"

/// Generic class for errors
class CError
{
public:
	/// Error code specific for domain
	typedef UInt32 ErrorCode;
	enum { 
		NO_ERROR = 0 ///< Code 0 is reserved to class CNoError
	};

	/// Severity is the level of error importance
	enum eSeverity
	{
		FATAL,   ///< Error that prevents module's activity
		ERROR,   ///< Error that prevents action and requires recovery
		WARNING, ///< Any error that not prevents current execution flow
		MESSAGE  ///< Any information that can be needed in log
	};

public:

	/// Quick check for NO_ERROR state
   bool isNoError() const { return getCode() == NO_ERROR; }

	/// Returns error code
	ErrorCode getCode() const { return mCode; }

	/// Returns error severity
	eSeverity getSeverity() const { return mSeverity; }

	/// Returns moduleID of module that issued error
	/// @return pointer to null-terminated character sequence
	char const* getModuleID() const { return mModuleID.c_str(); }

	/// Returns a generic description of error
	/// @return pointer null-terminated character sequence
	virtual char const* getDescription() const { return mDescription.c_str(); }

	/// Constructor of CError class
	/// @param code error code
	/// @param pModuleID pointer to null-terminated character sequence or NULL
	/// @param severity error severity
	/// @param pDescription pointer to null-terminated character sequence or NULL
	CError(ErrorCode code, char const* pModuleID, eSeverity severity = ERROR, const char* pDescription = "") :
		mCode(code),
		mSeverity(severity),
		mModuleID(pModuleID ? pModuleID : ""),
		mDescription(pDescription ? pDescription : "")
	{
	}

	/// Destructor of CError class
	virtual ~CError() { }

	virtual CError& operator=(CError const& err)
	{
	   mCode = err.mCode;
	   mSeverity = err.mSeverity;
	   mModuleID = err.mModuleID;
	   mDescription = err.mDescription;
	   return *this;
	}

	/// Comparison operator. Comparison is performed by error code.
	bool operator==(CError const & err) const
	{
		return getCode() == err.getCode();
	}

	/// Serialization operator 
	/// @todo
	virtual operator std::string() const
	{
		std::stringstream ss;
		ss << getCode() << " " << getModuleID() << " " << getDescription();
		return ss.str();
	}

	/// Returns the instance of CError with NO_ERROR state
	/// @param pModuleID pointer to null-terminated character sequence or NULL
	/// @param pDescription pointer to null-terminated character sequence or NULL
	static CError NoError(char const* pModuleID = "", char const* pDescription = "No error")
	{
	   return CError(CError::NO_ERROR, pModuleID, CError::MESSAGE, pDescription);
	}

	/// Returns string with description of errno code
	static std::string FormErrnoDescr(int e)
	{
	   std::stringstream ss;
	   ss << "Error " << e << ": " << strerror(e);
	   return ss.str();
	}

private:

	ErrorCode mCode;
	eSeverity mSeverity;
	std::string mModuleID;
	std::string mDescription;
};

/// Basic interface for error handler
class IErrorHandler
{
public:
	virtual void handleError(CError const & error) = 0;
	virtual ~IErrorHandler() {}
};

#endif // CERROR_HPP__
