/* 
 * iviLINK SDK, version 1.1.19
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 
 

#include <unistd.h>
#include <sys/param.h>

#include "AuthenticationDialog.hpp"
namespace authentication
{

Logger AuthenticationDialog::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("applications.AuthenticationApplication"));

#ifndef ANDROID
AuthenticationDialog::AuthenticationDialog(QWidget *parent)
   : QWidget(parent)
   , iviLink::CApp(iviLink::Service::Uid("AuthenticationService"))
   , SystemControllerMsgProxy("SysCtrl_AuthApp")
   , mParent(parent)
#else
AuthenticationDialog::AuthenticationDialog(iviLink::Android::AppInfo appInfo, JavaVM * pJm, jobject callbacksObj, std::string pathToTrlist)
   : iviLink::CApp(iviLink::Service::Uid("AuthenticationService"), appInfo)
   , SystemControllerMsgProxy("SysCtrl_AuthApp")
   , mAppInfo(appInfo)
   , mInternalPath(pathToTrlist)
   , mpJm(pJm)
   , jAppCallbacks(callbacksObj)
   , mPin("")
   , theirPin("")
#endif //ANDROID
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
}

void AuthenticationDialog::init()
{
   initInIVILink();

   MutexLocker lock(mMutex);
   switch2INITIAL();

#ifndef ANDROID
   char path[MAXPATHLEN];
   getcwd(path, MAXPATHLEN);
   mInternalPath = std::string(path);
   LOG4CPLUS_INFO(sLogger, "internal path is: "+mInternalPath);
   mpAuthenticationProxy = new CAuthenticationProxy(iviLink::Service::Uid("AuthenticationService"));
#else
   mpAuthenticationProxy = new CAuthenticationProxy(iviLink::Service::Uid("AuthenticationService"), 
                                                    mAppInfo);
#endif //ANDROID

   
#ifndef ANDROID
   setupUi(this); // QT stuff
   msgBox = new QDialog(this);
   msgBoxWidget = new QWidget(msgBox);
   QObject::connect( this, SIGNAL(showPopup(int, QString)), this, SLOT(on_showPopup(int, QString)));
   QObject::connect( this, SIGNAL(showPINCodeWindow()), this, SLOT(on_showPINCodeWindow()));
#endif //ANDROID

   CError err = SystemControllerMsgProxy::connect();
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(sLogger, "Unable to establish connection to system controller " + (std::string)err);
   }
}

void AuthenticationDialog::onInitDone(iviLink::ELaunchInfo launcher)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   MutexLocker lock(mMutex);
   if (iviLink::LAUNCHED_BY_USER == launcher)
   {
      LOG4CPLUS_INFO(sLogger, "started by user");
      registerProfileCallbacks(iviLink::Profile::ApiUid("AuthenticationProfile_PAPI_UID"),this);
      if(loadService(iviLink::Service::Uid("AuthenticationService")))
      {
         LOG4CPLUS_INFO(sLogger, "started by user - service loaded!");
      }
      mpAuthenticationProxy->sendPublicKey();
      CError err = mpAuthenticationProxy->getError();
      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(sLogger, static_cast<std::string>(err));
      }
   }
   else
   {
      LOG4CPLUS_INFO(sLogger, "started by iviLink");
   }
}

void AuthenticationDialog::onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   MutexLocker lock(mMutex);
   registerProfileCallbacks(iviLink::Profile::ApiUid("AuthenticationProfile_PAPI_UID"),this);
}

void AuthenticationDialog::onIncomingServiceAfterLoading(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   MutexLocker lock(mMutex);
   mpAuthenticationProxy->sendPublicKey();
   CError err = mpAuthenticationProxy->getError();
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(sLogger, static_cast<std::string>(err));
   }
}

bool AuthenticationDialog::checkPINs()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
#ifndef ANDROID
   if(sRemotePIN == sLocalPIN)
#else
   LOG4CPLUS_INFO(sLogger, "checkPINs(): mPin="+mPin);
   LOG4CPLUS_INFO(sLogger, "checkPINs(): theirPin="+theirPin);
   if(mPin.compare(theirPin) == 0)
#endif //ANDROID
   {
      LOG4CPLUS_WARN(sLogger, "Authentication is successful!!!");
      if(mState == WAITING_REMOTE_PIN)
      { 
         switch2WAITING_REMOTE_DEATH(); // master
      } 
      else if(mState == WAITING_LOCAL_PIN) 
      {
         switch2WAITING_PIN_CONFIRMATION();  // slave
      }
      return true;
   } 
   else
   {
      LOG4CPLUS_WARN(sLogger, "Authentication failed!!! PINs aren't equal");
      if(mState == WAITING_LOCAL_PIN || mState == WAITING_REMOTE_PIN)
      {
#ifndef ANDROID
         sLocalPIN.clear();
         pinEditLine->clear();
         emit showPopup(authentication::incorrectPin, QString("Authentication failed!!! PINs aren't equal. Try again."));

         backspaceButton->setEnabled(true);
         backspaceButton->setVisible(true);
         OKButton->setEnabled(true);
         OKButton->setVisible(true);
#else
         mPin = std::string("");
         callJavaMethod("resetPinDialog");
#endif //ANDROID
      }
      return false;
   }
   return false;
}

void AuthenticationDialog::gotPIN(int first_digit, int second_digit, int third_digit, int fourth_digit)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   MutexLocker lock(mMutex);
   LOG4CPLUS_INFO(sLogger, "gotPIN(" + convertIntegerToString(first_digit)
                                      + convertIntegerToString(second_digit)
                                      + convertIntegerToString(third_digit)
                                      + convertIntegerToString(fourth_digit) + ")");

#ifndef ANDROID
   sRemotePIN.setPIN(first_digit, second_digit, third_digit, fourth_digit);
#else
   theirPin = convertIntegerToString(first_digit)
            + convertIntegerToString(second_digit)
            + convertIntegerToString(third_digit)
            + convertIntegerToString(fourth_digit);
#endif //ANDROID

   if (mState != WAITING_REMOTE_PIN && mState != WAITING_ANY_PIN)
   {
      LOG4CPLUS_ERROR(sLogger, "got remote PIN in some wrong state");
      return;
   }
   
   if(mState == WAITING_ANY_PIN)
   {
      switch2WAITING_LOCAL_PIN(); // become slave
      return; 
   }
   
   if(mState == WAITING_REMOTE_PIN) checkPINs(); // we have local PIN already
}

void AuthenticationDialog::onAuthenticationIsNotRequired()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   MutexLocker lock(mMutex);
   if(getLaunchInfo() == iviLink::LAUNCHED_BY_USER) 
   {
      switch2WAITING_REMOTE_DEATH();
   } 
   else 
   {
      switch2SUCCESS();
   }
}

void AuthenticationDialog::onAuthenticationIsRequired()
{
   MutexLocker lock(mMutex);
   LOG4CPLUS_INFO(sLogger, "onAuthenticationIsRequired()");
   switch2WAITING_ANY_PIN();
}

#ifndef ANDROID
void AuthenticationDialog::on_showPINCodeWindow()
{
   LOG4CPLUS_INFO(sLogger, "on_showPINCodeWindow()");
   mParent->parentWidget()->show();
}

void AuthenticationDialog::on_showPopup(int popupType, QString message)
{
   LOG4CPLUS_INFO(sLogger, "on_showPopup(" + message.toStdString() + ")");

   msgBoxWidget->setFixedSize(QSize(328, 110));
   msgBox->setWindowFlags(Qt::FramelessWindowHint);
   msgBox->setAttribute(Qt::WA_TranslucentBackground);
   msgBox->setAttribute(Qt::WA_NoSystemBackground);

   switch (popupType)
   {
   case authentication::tooShortPin:
   {
       msgBoxWidget->setStyleSheet("image: url(:images/pin/enter_4_digits_msg.png) ");
       break;
   }
   case authentication::incorrectPin:
   {
       msgBoxWidget->setStyleSheet("image: url(:images/pin/auth_failed.png) ");
       break;
   }
   default:
   {}
   }

   QTimer *msgBoxCloseTimer = new QTimer(this);
   msgBoxCloseTimer->setInterval(3000);
   msgBoxCloseTimer->setSingleShot(true);
   QObject::connect(msgBoxCloseTimer, SIGNAL(timeout()), msgBox, SLOT(reject()));
   msgBoxCloseTimer->start();

   msgBox->adjustSize();
   /*int ret =*/ msgBox->exec();
   msgBoxCloseTimer->start();
}

void AuthenticationDialog::closePopup()
{
   msgBox->close();
}
#endif //ANDROID

#ifndef ANDROID
void AuthenticationDialog::on_OKButton_clicked()
#else
void AuthenticationDialog::okButtonClicked()
#endif //ANDROID
{
   LOG4CPLUS_INFO(sLogger, "on_OKButton_clicked()");
#ifndef ANDROID
   if(!sLocalPIN.isPINSet()) {
      emit showPopup(authentication::tooShortPin, QString("Wrong PIN input. Please try again."));
      return;
   }
#else
   // this will never happen because of a check in the java code
   if(mPin.size() != 4)  return; 
#endif // ANDROID
   if (mState == WAITING_ANY_PIN)
   {
      switch2WAITING_REMOTE_PIN(); //become master  
      return; // remote pin is not avaliable yet 
   }
   
   if(mState == WAITING_LOCAL_PIN) checkPINs(); // remote PIN was already here
}

#ifndef ANDROID
void AuthenticationDialog::on_CancelButton_clicked()
#else
void AuthenticationDialog::cancelButtonClicked()
#endif //ANDROID
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   SystemControllerMsgProxy::requestAuthenticationCanceled();
}

#ifndef ANDROID
void AuthenticationDialog::on_backspaceButton_clicked()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   sLocalPIN.popFromPIN();
   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_oneButton_clicked()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   sLocalPIN.pushToPIN(1);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_twoButton_clicked()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   sLocalPIN.pushToPIN(2);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_threeButton_clicked()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   sLocalPIN.pushToPIN(3);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_fourButton_clicked()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   sLocalPIN.pushToPIN(4);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_fiveButton_clicked()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   sLocalPIN.pushToPIN(5);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_sixButton_clicked()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   sLocalPIN.pushToPIN(6);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_sevenButton_clicked()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   sLocalPIN.pushToPIN(7);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_eightButton_clicked()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   sLocalPIN.pushToPIN(8);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_nineButton_clicked()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   sLocalPIN.pushToPIN(9);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_zeroButton_clicked()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   sLocalPIN.pushToPIN(0);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::closeEvent(QCloseEvent *event)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   event->ignore();
}
#endif //ANDROID


int AuthenticationDialog::getIntFromStr(std::string str, int pos)
{
   int result;
   std::stringstream strStream(str.substr(pos,1));
   strStream >> result;
   return result;
}


void AuthenticationDialog::onServiceDropped(iviLink::Service::Uid const&/* serviceUID*/)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__ );
   LOG4CPLUS_INFO(sLogger, "Counter-service was dropped by counter-side");
   // TODO fix when we are initial and counter-side has died - add prof api call
   switch2SUCCESS();
}


void AuthenticationDialog::switch2INITIAL() 
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(sLogger, "STATE:: INTIIAL");
   mState = INITIAL;
}

void AuthenticationDialog::switch2WAITING_ANY_PIN()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   if(mState == INITIAL)
   {
      LOG4CPLUS_INFO(sLogger, "STATE:: WAITING_ANY_PIN");
      mState = WAITING_ANY_PIN;
#ifndef ANDROID
      emit showPINCodeWindow();
#else
      callJavaMethod("showPinDialog");
#endif //ANDROID
   }
   else
   {
      LOG4CPLUS_ERROR(sLogger, "switch2WAITING_ANY_PIN()");
      return;
   }
}

void AuthenticationDialog::switch2WAITING_REMOTE_PIN() // is master
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   if(mState == WAITING_ANY_PIN)
   {
      LOG4CPLUS_INFO(sLogger, "STATE:: WAITING_REMOTE_PIN (our side generated pin first and we are master now)");
      mState = WAITING_REMOTE_PIN;
#ifndef ANDROID
      // TODO grey buttons
      backspaceButton->setEnabled(false);
		backspaceButton->setVisible(false);
		OKButton->setEnabled(false);
		OKButton->setVisible(false);
      mpAuthenticationProxy->sendPIN(sLocalPIN.getFirstDigit(),
                                   sLocalPIN.getSecondDigit(),
                                   sLocalPIN.getThirdDigit(),
                                   sLocalPIN.getFourthDigit());
#else
      mpAuthenticationProxy->sendPIN(getIntFromStr(mPin, 0),
                                    getIntFromStr(mPin, 1),
                                    getIntFromStr(mPin, 2),
                                    getIntFromStr(mPin, 3));
      callJavaMethod("lockText");
#endif //ANDROID
   }
   else
   {
      LOG4CPLUS_ERROR(sLogger, "switch2WAITING_REMOTE_PIN()");
   }
}

void AuthenticationDialog::switch2WAITING_LOCAL_PIN() // is slave
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   if(mState == WAITING_ANY_PIN)
   {
      LOG4CPLUS_INFO(sLogger, "STATE:: WAITING_LOCAL_PIN (other side generated pin first and we are slave now)");
      mState = WAITING_LOCAL_PIN;
   }
   else
   {
      LOG4CPLUS_ERROR(sLogger, "switch2WAITING_LOCAL_PIN()");
   }
}


void AuthenticationDialog::switch2WAITING_PIN_CONFIRMATION()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   if(mState == WAITING_LOCAL_PIN)
   {
      LOG4CPLUS_INFO(sLogger, "STATE:: WAITING_PIN_CONFIRMATION (everything is ok on this side!)");
      mState = WAITING_PIN_CONFIRMATION;
      // pins match, is trusted
      mpAuthenticationProxy->writeRemoteUIDToTrustList();
      LOG4CPLUS_INFO(sLogger, "switch2WAITING_PIN_CONFIRMATION: sending pin");
      // wait for confirmation
#ifndef ANDROID
      // TODO grey buttons
      OKButton->setEnabled(false);
      OKButton->setVisible(false);
      mpAuthenticationProxy->sendPIN(sLocalPIN.getFirstDigit(),
                                   sLocalPIN.getSecondDigit(),
                                   sLocalPIN.getThirdDigit(),
                                   sLocalPIN.getFourthDigit());
#else
      mpAuthenticationProxy->sendPIN(getIntFromStr(mPin, 0),
                                    getIntFromStr(mPin, 1),
                                    getIntFromStr(mPin, 2),
                                    getIntFromStr(mPin, 3));
      callJavaMethod("hideDialog");
#endif //ANDROID
   } 
   else
   {
      LOG4CPLUS_ERROR(sLogger, "switch2WAITING_PIN_CONFIRMATION()");
   }
}

void AuthenticationDialog::switch2SUCCESS()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   if(mState == WAITING_PIN_CONFIRMATION) 
   {
      LOG4CPLUS_INFO(sLogger, "STATE:: SUCCESS (got master's ok) (now we can tell SystemController about success)");
      mState = SUCCESS;
      authenticationSuccess();
   }
   else if(mState == INITIAL) 
   {
      LOG4CPLUS_INFO(sLogger, "STATE:: SUCCESS (auth nreq) (now we can tell SystemController about success)");
      mState = SUCCESS;
      authenticationSuccess();
   }
   else if(mState == WAITING_REMOTE_DEATH)
   {
      LOG4CPLUS_INFO(sLogger, "STATE:: SUCCESS (our slave has died) (now we can tell SystemController about success)");
      mState = SUCCESS;
      authenticationSuccess();
   }
   else
   {
      LOG4CPLUS_ERROR(sLogger, "switch2SUCCESS()");
   }
}

void AuthenticationDialog::switch2WAITING_REMOTE_DEATH()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   if(mState == WAITING_REMOTE_PIN || mState == INITIAL)
   {
      // pins match, is trusted
      if (mState == WAITING_REMOTE_PIN)
      {
         mpAuthenticationProxy->writeRemoteUIDToTrustList();
         // notify slave
         sendExternalState(ext_AUTH_OK);
#ifndef ANDROID
#else
         callJavaMethod("hideDialog");
#endif //ANDROID
      }
      mState = WAITING_REMOTE_DEATH;
   }
   else
   {
      LOG4CPLUS_ERROR(sLogger, "switch2WAITING_REMOTE_DEATH");
   }
}

void AuthenticationDialog::authenticationSuccess()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   SystemControllerMsgProxy::requestAuthenticationOK(); 
}

void AuthenticationDialog::sendExternalState(ExternalStates state)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   mpAuthenticationProxy->sendExternalState((int)state);
}

void AuthenticationDialog::onExternalStateCame(int extState)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   if(extState == (int)ext_AUTH_OK)
   {
      switch2SUCCESS();
   }
}

#ifndef ANDROID
#else
void AuthenticationDialog::setPin(std::string pin)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   MutexLocker lock(mMutex);
   LOG4CPLUS_INFO(sLogger, "mPin size = "+convertIntegerToString(pin.size()));
   mPin = pin;
   LOG4CPLUS_INFO(sLogger, "mPin="+mPin);
}

void AuthenticationDialog::callJavaMethod(const char * name)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_TRACE(sLogger, name);
   iviLink::Android::JniThreadHelper jth(mpJm);
   JNIEnv * env = jth.getEnv();
   jclass claz = env->GetObjectClass(jAppCallbacks);
   jmethodID mid = env->GetMethodID(claz, name, "()V");
   env->CallVoidMethod(jAppCallbacks, mid);
}
#endif

} /*namespace authentication*/
