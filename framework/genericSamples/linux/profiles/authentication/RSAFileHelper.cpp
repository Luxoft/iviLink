
#include "RSAFileHelper.hpp"

RSAFileHelper::RSAFileHelper(std::string const& fileName)
{
    mFileName = fileName;
    mInFile.open(fileName.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
}

RSAFileHelper::~RSAFileHelper()
{
    mInFile.close();
}

bool RSAFileHelper::isFilePresent()
{
    return mInFile.good();
}

UInt32 RSAFileHelper::getDataSize()
{
    if(!isFilePresent())
    {
        return 0;
    }
    mInFile.seekg(0, std::ios::end);
    int size = mInFile.tellg();
    mInFile.seekg(0, std::ios::beg);
    return size;
}


void RSAFileHelper::readDataToBuffer(char * buffer, const UInt32 bufSize)
{
    mInFile.read(buffer, bufSize);
}

void RSAFileHelper::writeDataToFile(const char * data, const UInt32 size, std::string const& fileName)
{
    std::ofstream outputBuffer(fileName.c_str(), std::ios::binary|std::ios::out);
    outputBuffer.write(data, size);
    outputBuffer.close();
}