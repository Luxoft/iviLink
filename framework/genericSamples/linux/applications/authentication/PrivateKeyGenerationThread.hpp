#ifndef PRIVATE_KEY_GENERATION_THREAD
#define PRIVATE_KEY_GENERATION_THREAD

#include "CThread.hpp"
#include "CAuthenticationProxy.hpp"
#include "Logger.hpp"

namespace iviLink
{
namespace Authentication
{

class PrivateKeyGenerationThread : public CThread
{
public:
    PrivateKeyGenerationThread(CAuthenticationProxy * proxy);
    virtual ~PrivateKeyGenerationThread() {}

    virtual void threadFunc();

private:
    CAuthenticationProxy * mAuthenticationProxy;
    static Logger mLogger;
};

}
}

#endif //PRIVATE_KEY_GENERATION_THREAD