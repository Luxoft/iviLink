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
#ifndef __FAKE_CALLBACKS_HPP__
#define __FAKE_CALLBACKS_HPP__

#include <jni.h>

#include "BaseProfileApi.hpp"

/**
 * Stores reference to a jobject passed later to a profile implementation as application callbacks
 * The reference is never freed, as currently there is no mechanism to unregister profile callbacks.
 */

class NonnativeCallbacksWrapper: public iviLink::Profile::IProfileCallbackProxy
{
public:
    NonnativeCallbacksWrapper(jobject object)
    {
        mCallbacksImplementation = object;
    }

    jobject getCallbacksImplementation()
    {
        return mCallbacksImplementation;
    }

    virtual void handleError(const BaseError&)
    {
    }

    virtual ~NonnativeCallbacksWrapper()
    {
    }

private:
    jobject mCallbacksImplementation;
};

#endif //__FAKE_CALLBACKS_HPP__
