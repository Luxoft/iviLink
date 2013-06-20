/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 *
 * Provides thread-safe method to write to a pipe descriptor.
 */


#ifdef __APPLE__
#ifndef __iviLinkCoreStack__PipeWriter__
#define __iviLinkCoreStack__PipeWriter__

#include <tr1/memory>

#include "Types.hpp"
#include "CMutex.hpp"
#include "Logger.hpp"

namespace iviLink
{
    namespace Ipc
    {
        class PipeWriter
        {
        public:
            PipeWriter(int pipeWriteDesriptor);
            ~PipeWriter();
            /**
             * Writes data to mPipeWriteDescriptor
             * @return false if the pipe is broken
             */
            bool writeToPipe(UInt8 const * const data, UInt32 dataSize);
        private:
            int mPipeWriteDescriptor;
            CMutex mPipeWriteMutex;
            bool mIsBroken;
            static Logger mLogger;
        };
        
        typedef std::tr1::shared_ptr<PipeWriter> tPipeWriterPtr;
    }
}

#endif /* defined(__iviLinkCoreStack__PipeWriter__) */

#endif
