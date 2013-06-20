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
