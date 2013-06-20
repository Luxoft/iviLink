
#include "RSAFileHelper.hpp"

static NSFileManager * fileManager = [NSFileManager defaultManager];

RSAFileHelper::RSAFileHelper(std::string const& fileName)
{
    mFileName = fileName;
}

RSAFileHelper::~RSAFileHelper()
{
}

bool RSAFileHelper::isFilePresent()
{
    return [fileManager fileExistsAtPath: [NSString stringWithCString : mFileName.c_str()]];
}

UInt32 RSAFileHelper::getDataSize()
{
    if(!isFilePresent())
    {
        return 0;
    }
    NSData * dataBuffer = [fileManager contentsAtPath: [NSString stringWithCString : mFileName.c_str()]];
    return [dataBuffer length];
}


void RSAFileHelper::readDataToBuffer(char * buffer, const UInt32 bufSize)
{
    NSData * dataBuffer = [fileManager contentsAtPath: [NSString stringWithCString : mFileName.c_str()]];
    [dataBuffer getBytes: buffer length: bufSize];
}

void RSAFileHelper::writeDataToFile(const char * data, const UInt32 size, std::string const& fileName)
{
    NSData * nsdata = [[NSData alloc] initWithBytes:data length:size];
    NSString * nsFileName = [NSString stringWithCString:fileName.c_str()];
    if (![fileManager fileExistsAtPath: nsFileName])
    {
        [fileManager createFileAtPath: nsFileName contents:nsdata attributes:nil];
    }
    else
    {
        [nsdata writeToFile: nsFileName atomically:YES];
    }
}