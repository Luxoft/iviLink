#include "PrivateKeyGenerationThread.hpp"
using namespace iviLink::Authentication;
Logger PrivateKeyGenerationThread::mLogger=Logger::getInstance("PrivateKeyGenerationThread");

PrivateKeyGenerationThread::PrivateKeyGenerationThread(CAuthenticationProxy * proxy)
: CThread("PrivateKeyGenerationThread")
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mAuthenticationProxy = proxy;
}

void PrivateKeyGenerationThread::threadFunc()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mAuthenticationProxy->sendPublicKey();
}