/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
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
#ifndef QMLAMLAUTHENTICATIONDIALOG_HPP
#define QMLAMLAUTHENTICATIONDIALOG_HPP
#ifndef ANDROID
#include <QObject>
#include <QString>

#include "IAuthenticationDialog.hpp"
#include "AuthenticationStateMachine.hpp"
#include "PINCode.hpp"
#include "Logger.hpp"

namespace iviLink
{
namespace Authentication
{

class QMLAuthenticationDialog : public QObject,
        public IAuthenticationDialog
{
    Q_OBJECT
public:
    QMLAuthenticationDialog();

    void init();

    ~QMLAuthenticationDialog();
    virtual void showNumPad();
    virtual void hideNumPad();
    virtual void showRetryPopup();
    virtual void resetNumPad();
    virtual void lockInput();
    virtual void unlockInput();

public slots:
    void onNumberClicked(int number);
    void onOKButtonClicked();
    void onCancelButtonClicked();
    void onBackspaceButtonClicked();

signals:
    void sigShowNumPad();
    void sigHideNumPad();
    void sigShowRetryPopup();
    void sigResetNumpad();
    void sigLockInput();
    void sigUnlockInput();
    void sigLockOkButton();
    void sigUnlockOkButton();
    void sigLockBackspaceButton();
    void sigUnlockBackspaceButton();
    void sigSetTexbox(QString text);
private:
    void updateUiAvaliability();

    static Logger mLogger;
    PINCode mPin;
    AuthenticationStateMachine* mMachine;
};

}
}

#endif //ANDROID
#endif // QMLAMLAUTHENTICATIONDIALOG_HPP
