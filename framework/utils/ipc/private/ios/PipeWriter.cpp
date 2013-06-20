/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */


#ifdef __APPLE__

#include <sys/param.h>
#include <unistd.h>
#include "PipeWriter.hpp"

namespace iviLink
{
    namespace Ipc
    {
        Logger PipeWriter::mLogger = Logger::getInstance("iviLink::IPC::iOS::PipeWriter");
        PipeWriter::PipeWriter(int pipeWriteDescr)
        {
            LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
            LOG4CPLUS_INFO(mLogger, "Pipe descriptor: " + convertIntegerToString(pipeWriteDescr));
            mPipeWriteDescriptor = pipeWriteDescr;
            mIsBroken = false;
        }
        
        PipeWriter::~PipeWriter()
        {
            LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
            close(mPipeWriteDescriptor);
        }
        
        bool PipeWriter::writeToPipe(const UInt8 *const data, UInt32 dataSize)
        {
            LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
            LOG4CPLUS_INFO(mLogger, "total size to write: " + convertIntegerToString(dataSize));
            MutexLocker lock(mPipeWriteMutex);
            if (mIsBroken)
            {
                LOG4CPLUS_ERROR(mLogger, "Pipe is broken already");
                return false;
            }
            int wroteSize = 0;
            while (wroteSize != dataSize)
            {
                int size = write(mPipeWriteDescriptor, data + wroteSize, dataSize - wroteSize);
                LOG4CPLUS_INFO(mLogger, "wrote chunk size = " + convertIntegerToString(size));
                if (size == -1)
                {
                    LOG4CPLUS_ERROR(mLogger, "Broken pipe!");
                    mIsBroken = true;
                    return false;
                }
                else
                {
                    wroteSize += size;
                    LOG4CPLUS_INFO(mLogger, "wrote to pipe: " + convertIntegerToString(wroteSize) + " out of " + convertIntegerToString(dataSize));
                }
            }
            return true;
        }
    }
}

#endif