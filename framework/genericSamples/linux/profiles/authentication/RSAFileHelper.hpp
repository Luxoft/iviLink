
#ifndef __iviLink_Framework__RSAFileHelper__
#define __iviLink_Framework__RSAFileHelper__

#include <string>
#include <sstream>
#include <fstream>
#include "Types.hpp"

class RSAFileHelper
{
public:
    static void writeDataToFile(const char * data, const UInt32 size, std::string const& fileName);
    
    RSAFileHelper(std::string const& filePath);
    bool isFilePresent();
    UInt32 getDataSize();
    void readDataToBuffer(char * buffer, const UInt32 bufSize);
    ~RSAFileHelper();
private:
    std::ifstream mInFile;
    std::string mFileName;
};
#endif /* defined(__iviLink_Framework__File__) */
