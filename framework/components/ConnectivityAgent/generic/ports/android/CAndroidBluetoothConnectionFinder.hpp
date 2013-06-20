#ifndef ANDROID_BLUETOOTH_FINDER_HPP
#define ANDROID_BLUETOOTH_FINDER_HPP

#include "CConnectionFinder.hpp"
#include "Logger.hpp"
#include "ConnectionInfo.hpp"
#include <jni.h>

#ifndef ANDROID
#else  // android-specific implementation
namespace iviLink
{
   namespace ConnectivityAgent
   {
      namespace HAL
      {
         /// CTcpAutoConnectionFinder class
         /**
          *
          */
          class CAndroidBluetoothConnectionFinder : public CConnectionFinder
         {
         private:
            static Logger logger;

         public:
            // Methods section

            /**
             * Constructor
             * @param handler object which will process found connections
             * @param gender gender of connection
             */
            CAndroidBluetoothConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender);

            /**
             * Destructor
             */
            virtual ~CAndroidBluetoothConnectionFinder();

            virtual void setAutoConnectionAllowed(bool) { };

         protected:
            // Methods section
            // from CConnectionFinder

            virtual void performSearch();


         private:

            bool isValid;

            IFoundConnectionHandler * handler;

            JavaVM * mpJM; 
            jobject jImplementation; // object with callbacks for using Java Bluetooth API
            jmethodID performSearchJava;
         };

      }
   }
}

#endif // ANDROID
#endif // CAndroidBluetoothConnectionFinder
