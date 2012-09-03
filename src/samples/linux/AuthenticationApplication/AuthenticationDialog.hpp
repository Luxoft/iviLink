/* 
 * 
 * iviLINK SDK, version 1.1.2
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
 * 
 */







#ifndef CAUTHENTICATION_DIALOG_HPP
#define CAUTHENTICATION_DIALOG_HPP

#ifndef ANDROID
#include <QMessageBox>
#include <QCloseEvent>
#include <QTimer>
#include "ui_auth.h"
#include "samples/linux/AuthenticationApplication/CPINCode.hpp"
#else
#include "utils/android/JniThreadHelper.hpp"
#include <cassert>
#include <jni.h>
#endif //ANDROID

#include "utils/misc/Logger.hpp"
#include "AuthStates.hpp"
#include "MutexLocker.hpp"
#include "framework/messageProtocol/SystemController_Authentication/Authentication/SystemControllerMsgProxy.hpp"
#include "framework/public/appLib/CApp.hpp"
#include "samples/linux/Profiles/ProfileProxy/CAuthenticationProxy.hpp"

namespace authentication
{

#ifndef ANDROID
enum popupMessageTypes
{
    tooShortPin = 0,
    incorrectPin = 1
};

class AuthenticationDialog : public QWidget
                           , private Ui_AuthenticationForm
                           , private iviLink::CApp
                           , public IAuthenticationProfile_API::Callbacks
                           , public AuthenticationAppMsgProtocol::SystemControllerMsgProxy
{
   Q_OBJECT
#else

class AuthenticationDialog : private iviLink::CApp
                           , public IAuthenticationProfile_API::Callbacks
                           , public AuthenticationAppMsgProtocol::SystemControllerMsgProxy
{
#endif //ANDROID

private:
   static Logger sLogger;

public:

  #ifndef ANDROID
   AuthenticationDialog(QWidget *parent = 0);

   virtual void closeEvent(QCloseEvent *event);
   #else

   AuthenticationDialog(iviLink::Android::AppInfo appInfo, JavaVM * pJm, jobject callbacksObj, std::string pathToTrlist);

   #endif //ANDROID

   virtual void onRequestShutDown();

   /**
    * Callback is called after initing of app
    * @param launcher gives information who launched app (user of iviLink)
    */
   virtual void initDone(iviLink::ELaunchInfo launcher);

   /**
    * Callback is called before loading incoming service
    * @param service is UID of service
    */
   virtual void incomingServiceBeforeLoading(const iviLink::Service::Uid &service);

   /**
    * Callback is called after loading of incoming service
    */
   virtual void incomingServiceAfterLoading(const iviLink::Service::Uid &service);

   /**
    * Callback is called in case of and service session
    * @param service UID is uid of service
    */
   virtual void serviceDropped(const iviLink::Service::Uid &service);

   virtual void handleError(CError const & error)
   {
      LOG4CPLUS_INFO(sLogger,static_cast<std::string>(error));
   }

   virtual void gotPIN(int first_digit, int second_digit, int third_digit, int fourth_digit);
   virtual void onAuthenticationIsNotRequired();
   virtual void onAuthenticationIsRequired();
   virtual void onExternalStateCame(int state);

private:
   CAuthenticationProxy * mpAuthenticationProxy;

  #ifndef ANDROID
  CPINCode sLocalPIN;
  CPINCode sRemotePIN;
  QDialog *msgBox;
  QWidget *msgBoxWidget;
  QWidget* mParent;
  #else
  JavaVM * mpJm;
  jobject jAppCallbacks;
  iviLink::Android::AppInfo mAppInfo;
   
  std::string mPin;
  std::string theirPin;
  std::string mPathToTrlist;
  #endif

  bool checkPINs();
  int getIntFromStr(std::string str, int pos);

#ifndef ANDROID
private slots:
   void closePopup();
   void on_OKButton_clicked();
   void on_CancelButton_clicked();

   void on_backspaceButton_clicked();
   void on_oneButton_clicked();
   void on_twoButton_clicked();
   void on_threeButton_clicked();
   void on_fourButton_clicked();
   void on_fiveButton_clicked();
   void on_sixButton_clicked();
   void on_sevenButton_clicked();
   void on_eightButton_clicked();
   void on_nineButton_clicked();
   void on_zeroButton_clicked();

   void on_showPopup(int popupType, QString message);
   void on_showPINCodeWindow();

signals:
   void showPopup(int popupType, QString message);
   void showPINCodeWindow();
#else
public:
  void okButtonClicked();
  void cancelButtonClicked();
  void callJavaMethod(const char * methodName);
  void setPin(std::string pin);   
  std::string getPathToTrlist()
   {
      return mPathToTrlist;
   }
#endif //ANDROID
   
   
private:
   LocalStates mState;
   CMutex mMutex;

  void switch2INITIAL(); // initial state
  void switch2WAITING_ANY_PIN();
  void switch2WAITING_REMOTE_PIN(); // is master
  void switch2WAITING_LOCAL_PIN(); // is slave
  void switch2WAITING_PIN_CONFIRMATION();
  void switch2WAITING_REMOTE_DEATH();
  void switch2SUCCESS();
   
   void authenticationSuccess();
   void sendExternalState(ExternalStates state);
};

} /* namespace authentication */

#endif /*CAUTHENTICATION_DIALOG_HPP*/
