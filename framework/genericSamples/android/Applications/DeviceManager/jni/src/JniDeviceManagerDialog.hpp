#ifndef JNI_DEVICE_MANAGER_DIALOG_HPP
#define JNI_DEVICE_MANAGER_DIALOG_HPP

#include <jni.h>
#include "IDeviceManagerDialog.hpp"
#include "Logger.hpp"
#include "FoundDevice.hpp"
#include "DeviceManager.hpp"

namespace iviLink
{
namespace DeviceManager
{

class JniDeviceManagerDialog : public IDeviceManagerDialog
{

public:
    JniDeviceManagerDialog(JavaVM* jm, jobject callbacks);
    virtual ~JniDeviceManagerDialog();

    //from the IDeviceManagerDialog interface
    virtual void setDeviceManager(DeviceManager* deviceManager);
    virtual void updateListContent(CTrustList::tNameUidVector);

    virtual void searchForDevices() {};
    virtual void disconnectDevice(int index);
    virtual void updateFoundListContent(std::vector<iviLink::FoundDevice>) {};

private:
    void callJavaMethod(const char * methodName);
    JavaVM * mJM;
    jobject mCallbacks;
    Logger mLogger;
    DeviceManager * mDeviceManager;
    
};

}
}
#endif //JNI_DEVICE_MANAGER_DIALOG_HPP
