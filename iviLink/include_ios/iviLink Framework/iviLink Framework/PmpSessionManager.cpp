/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 *
 * Waiting services logic.
 */
#include <cassert>

#include "PmpSessionManager.hpp"
#include "PmpCommunicator.hpp"
#include "ByteOrder.hpp"

namespace iviLink
{
namespace PMP
{

Logger PmpSessionManager::mLogger = Logger::getInstance("iviLink::PMP::iOS::PmpSessionManager");

PmpSessionManager::PmpSessionManager()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mRoleNumber = 0;
    mOtherSideRoleNumber = 0;
    mRole = PMP_ROLE_NONE;
}

PmpSessionManager::~PmpSessionManager()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void PmpSessionManager::processFrame(PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (frame->client != PMP_CLIENT_SESSION)
    {
        LOG4CPLUS_INFO(mLogger, "wring client type");
        assert(false);
    }
    switch (frame->reqType)
    {
    case PMP_REQ_SESSION_NEGOTIATE_NUMBER:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_SESSION_NEGOTIATE_NUMBER");
        UInt32 number;
        memcpy(&number, frame->data, sizeof(UInt32));
        number = ByteOrder::ntoh32(number);
        onGotNegotiateNumber(number);
        break;
    case PMP_REQ_SESSION_SEND_WAITING_SERVICES:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_SESSION_SEND_WAITING_SERVICES");
        onGotWaitingServicesList(); // TODO ???
        break;
    default:
        LOG4CPLUS_FATAL(mLogger, "Unknown event: " + convertIntegerToString(frame->reqType));
        assert(false);
        break;
    }
}

void PmpSessionManager::genAndSendRoleNumber()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    srand(clock());
    mRoleNumber = abs(rand() + 1);
    UInt32 size = sizeof(PMPFrame) + sizeof(UInt32);
    PMPFrame * request = reinterpret_cast<PMPFrame*>(new UInt8[size]);
    request->size = size;
    request->reqType = PMP_REQ_SESSION_NEGOTIATE_NUMBER;
    request->client = PMP_CLIENT_SESSION;
    UInt32 roleNumberEndian = ByteOrder::hton32(mRoleNumber);
    memcpy(request->data, &roleNumberEndian, sizeof(UInt32));
    PmpCommunicator::getInstance()->sendFrame(request);
}

void PmpSessionManager::onGotNegotiateNumber(UInt32 number)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "Other side number: " + convertIntegerToString(number));
    LOG4CPLUS_INFO(mLogger, "My side number: " + convertIntegerToString(mRoleNumber));
    mOtherSideRoleNumber = number;
    checkRole();
}

void PmpSessionManager::onSendNegotiateNumber()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    genAndSendRoleNumber();
    checkRole();
}

void PmpSessionManager::onGotWaitingServicesList()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_WARN(mLogger, "Not implemented");
}

void PmpSessionManager::checkRole()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (mRoleNumber != 0 && mOtherSideRoleNumber != 0)
    {
        if (mRoleNumber == mOtherSideRoleNumber)
        {
            genAndSendRoleNumber();
            LOG4CPLUS_TRACE_METHOD(mLogger, "EQUAL");
        } else
        {
            if (mRoleNumber > mOtherSideRoleNumber)
            {
                mRole = PMP_ROLE_MASTER;
                LOG4CPLUS_TRACE_METHOD(mLogger, "PMP_ROLE_MASTER");
            } else
            {
                mRole = PMP_ROLE_SLAVE;
                LOG4CPLUS_TRACE_METHOD(mLogger, "PMP_ROLE_SLAVE");
            }
        }
    } else
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, "don't have one of the numbers yet");
    }
}
}
}
