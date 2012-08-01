/* 
 * 
 * iviLINK SDK, version 1.0
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



#include "samples/linux/AuthenticationApplication/AuthenticationDialog.hpp"

namespace authentication
{

Logger AuthenticationDialog::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("applications.AuthenticationApplication"));

AuthenticationDialog::AuthenticationDialog(QWidget *parent)
   : QWidget(parent)
   , iviLink::CApp(iviLink::Service::Uid("AuthenticationService"))
   , SystemControllerMsgProxy("SysCtrl_AuthApp")
   , mAuthenticationProxy(iviLink::Service::Uid("AuthenticationService"))
   , mParent(parent)
{
   registerProfileCallbacks(iviLink::Profile::ApiUid("AuthenticationProfile_PAPI_UID"),this);
   setupUi(this);

   CError err = SystemControllerMsgProxy::connect();
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(sLogger, "Unable to establish connection to system controller " + (std::string)err);
   }

   msgBox = new QDialog(this);
   msgBoxWidget = new QWidget(msgBox);

   QObject::connect( this, SIGNAL(showPopup(int, QString)), this, SLOT(on_showPopup(int, QString)));
   QObject::connect( this, SIGNAL(showPINCodeWindow()), this, SLOT(on_showPINCodeWindow()));
}

void AuthenticationDialog::initDone(iviLink::ELaunchInfo launcher)
{
   if (iviLink::LAUNCHED_BY_USER == launcher)
   {
      LOG4CPLUS_INFO(sLogger, "started by user");
      registerProfileCallbacks(iviLink::Profile::ApiUid("AuthenticationProfile_PAPI_UID"),this);
      assert(loadService(iviLink::Service::Uid("AuthenticationService")));
      mAuthenticationProxy.sendPublicKey();
      CError err = mAuthenticationProxy.getError();
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

void AuthenticationDialog::incomingServiceBeforeLoading(const iviLink::Service::Uid &service)
{
   registerProfileCallbacks(iviLink::Profile::ApiUid("AuthenticationProfile_PAPI_UID"),this);
}

void AuthenticationDialog::incomingServiceAfterLoading(const iviLink::Service::Uid &service)
{
   mAuthenticationProxy.sendPublicKey();
   CError err = mAuthenticationProxy.getError();
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(sLogger, static_cast<std::string>(err));
   }
}

bool AuthenticationDialog::checkPINs()
{
   LOG4CPLUS_INFO(sLogger, "checkPINs()");

   if(sRemotePIN.isPINSet() && sLocalPIN.isPINSet())
   {
      if(sRemotePIN == sLocalPIN)
      {
         LOG4CPLUS_WARN(sLogger, "Authentication is successful!!!");

         return true;
      }
      else
      {
         LOG4CPLUS_WARN(sLogger, "Authentication failed!!! PINs aren't equal");

         sLocalPIN.clear();
         pinEditLine->clear();

         emit showPopup(authentication::incorrectPin, QString("Authentication failed!!! PINs aren't equal. Try again."));

         return false;
      }
   }
   else
   {
      if(!sLocalPIN.isPINSet())
      {
         LOG4CPLUS_WARN(sLogger, "Local PIN is not set yet");

         return false;
      }
      else if(!sRemotePIN.isPINSet())
      {
         LOG4CPLUS_WARN(sLogger, "Remote PIN is not set yet");

         return false;
      }
   }

   return false;
}

void AuthenticationDialog::gotPIN(int first_digit, int second_digit, int third_digit, int fourth_digit)
{
    LOG4CPLUS_INFO(sLogger, "gotPIN(" + convertIntegerToString(first_digit)
                                      + convertIntegerToString(second_digit)
                                      + convertIntegerToString(third_digit)
                                      + convertIntegerToString(fourth_digit) + ")");

   sRemotePIN.setPIN(first_digit, second_digit, third_digit, fourth_digit);

   if(checkPINs())
   {
      mAuthenticationProxy.writeRemoteUIDToTrustList();
      SystemControllerMsgProxy::requestAuthenticationOK();
      unloadService(iviLink::Service::Uid("AuthenticationService"));

   }
}

void AuthenticationDialog::onAuthenticationIsNotRequired()
{
   LOG4CPLUS_INFO(sLogger, "onAuthenticationIsNotRequired(");

   SystemControllerMsgProxy::requestAuthenticationOK();
   unloadService(iviLink::Service::Uid("AuthenticationService"));

}

void AuthenticationDialog::onAuthenticationIsRequired()
{
   LOG4CPLUS_INFO(sLogger, "onAuthenticationIsRequired()");

   emit showPINCodeWindow();
}

void AuthenticationDialog::onRequestShutDown()
{
   LOG4CPLUS_INFO(sLogger, "onRequestShutDown()");

   LOG4CPLUS_INFO(sLogger, "Shutdown request from system controller - close Application");

   exit(0);
}

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

void AuthenticationDialog::on_OKButton_clicked()
{
   LOG4CPLUS_INFO(sLogger, "on_OKButton_clicked()");

   if(sLocalPIN.isPINSet())
   {
      mAuthenticationProxy.sendPIN(sLocalPIN.getFirstDigit(),
                                   sLocalPIN.getSecondDigit(),
                                   sLocalPIN.getThirdDigit(),
                                   sLocalPIN.getFourthDigit());
      if(checkPINs())
      {
         mAuthenticationProxy.writeRemoteUIDToTrustList();
         SystemControllerMsgProxy::requestAuthenticationOK();
         unloadService(iviLink::Service::Uid("AuthenticationService"));

      }
   }
   else
   {
      LOG4CPLUS_ERROR(sLogger, "Wrong PIN input.");

      sLocalPIN.clear();
      pinEditLine->clear();

      emit showPopup(authentication::tooShortPin, QString("Wrong PIN input. Please try again."));
   }
}

void AuthenticationDialog::on_CancelButton_clicked()
{
   LOG4CPLUS_INFO(sLogger, "on_CancelButton_clicked()");

   SystemControllerMsgProxy::requestAuthenticationCanceled();
   unloadService(iviLink::Service::Uid("AuthenticationService"));
}

void AuthenticationDialog::on_backspaceButton_clicked()
{
   LOG4CPLUS_INFO(sLogger, "on_backspaceButton_clicked()");
   sLocalPIN.popFromPIN();

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_oneButton_clicked()
{
   LOG4CPLUS_INFO(sLogger, "on_oneButton_clicked()");
   sLocalPIN.pushToPIN(1);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_twoButton_clicked()
{
   LOG4CPLUS_INFO(sLogger, "on_twoButton_clicked()");
   sLocalPIN.pushToPIN(2);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_threeButton_clicked()
{
   LOG4CPLUS_INFO(sLogger, "on_threeButton_clicked()");
   sLocalPIN.pushToPIN(3);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_fourButton_clicked()
{
   LOG4CPLUS_INFO(sLogger, "on_fourButton_clicked()");
   sLocalPIN.pushToPIN(4);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_fiveButton_clicked()
{
   LOG4CPLUS_INFO(sLogger, "on_fiveButton_clicked()");
   sLocalPIN.pushToPIN(5);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_sixButton_clicked()
{
   LOG4CPLUS_INFO(sLogger, "on_sixButton_clicked()");
   sLocalPIN.pushToPIN(6);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_sevenButton_clicked()
{
   LOG4CPLUS_INFO(sLogger, "on_sevenButton_clicked()");
   sLocalPIN.pushToPIN(7);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_eightButton_clicked()
{
   LOG4CPLUS_INFO(sLogger, "on_eightButton_clicked()");
   sLocalPIN.pushToPIN(8);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_nineButton_clicked()
{
   LOG4CPLUS_INFO(sLogger, "on_nineButton_clicked()");
   sLocalPIN.pushToPIN(9);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::on_zeroButton_clicked()
{
   LOG4CPLUS_INFO(sLogger, "on_zeroButton_clicked()");
   sLocalPIN.pushToPIN(0);

   LOG4CPLUS_INFO(sLogger, "PIN = " + sLocalPIN.getEnteredDigitsAsString());
   pinEditLine->setText(QString::fromStdString(sLocalPIN.getEnteredDigitsAsString()));
}

void AuthenticationDialog::closeEvent(QCloseEvent *event)
{
   LOG4CPLUS_INFO(sLogger, "closeEvent()");

   unloadService(iviLink::Service::Uid("AuthenticationService"));

   event->ignore();
}


void AuthenticationDialog::serviceDropped(iviLink::Service::Uid const&/* serviceUID*/)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__ );

   LOG4CPLUS_INFO(sLogger, "Counter-service was dropped by counter-side");
}


} /*namespace authentication*/
