
#ifndef ANDROID
#else

#include <cassert>

#include "CAndroidBluetoothConnectionFinder.hpp"
#include "CAndroidBluetoothAdapter.hpp"
#include "GlobalJavaVmGetter.h"
#include "JniThreadHelper.hpp"
#include "BluetoothObject.h"

using namespace iviLink::ConnectivityAgent::HAL;


Logger CAndroidBluetoothConnectionFinder::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CAndroidBluetoothConnectionFinder"));


CAndroidBluetoothConnectionFinder::CAndroidBluetoothConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender)
:
   CConnectionFinder(handler, gender)
{
 	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
 	mpJM = globalGetJavaVM();
 	jImplementation = getBluetoothHelperInstance();
 	if(jImplementation == NULL)
 	{
 		LOG4CPLUS_ERROR(logger, "jImplementation is NULL, isValid = false");
 		isValid = false;
 		return;
 	}
 	isValid = true;
	iviLink::Android::JniThreadHelper jth(mpJM);
	JNIEnv * env = jth.getEnv();
	assert(env);
	jclass claz = env->GetObjectClass(jImplementation);
	performSearchJava = env->GetMethodID(claz, "performSearch", "()I");
	assert(performSearchJava>=0);
}


CAndroidBluetoothConnectionFinder::~CAndroidBluetoothConnectionFinder()
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);	
}

void CAndroidBluetoothConnectionFinder::performSearch()
{
	if (!isValid) 
	{
		sleep(1000);
		LOG4CPLUS_ERROR(logger, "is not valid, sleeping");
		return;
	}
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	iviLink::Android::JniThreadHelper jth(mpJM);
	JNIEnv * env = jth.getEnv();
	int result = (int)env->CallIntMethod(jImplementation, performSearchJava);
	LOG4CPLUS_INFO(logger, "performSearch returned: ");
	if(result != 0) {
		LOG4CPLUS_INFO(logger, "performSearch returned true (found and connected)");
		CBluetoothInfo info;
		if(result == 1) {
			info.isServer = true;
		} else {
			info.isServer = false;
		}
   	    CAndroidBluetoothAdapter * pAdapter = new CAndroidBluetoothAdapter(info);
		setReady(pAdapter);
	} else if (result == 0) {
		LOG4CPLUS_INFO(logger, "performSearch returned false (not found anything)");
	}
}


#endif //ANDROID