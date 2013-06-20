#ifndef QMLAMLAUTHENTICATIONDIALOG_HPP
#define QMLAMLAUTHENTICATIONDIALOG_HPP
#ifndef ANDROID
#include <QObject>
#include <QString>
#include <QDesktopWidget>
#include <QDeclarativeView>

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
    virtual void setDeclarativeView(QDeclarativeView * declarativeView);

public slots:
    void onNumberClicked(int number);
    void onOKButtonClicked();
    void onCancelButtonClicked();
    void onBackspaceButtonClicked();
    void onRightButtonClicked();
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
    bool isCursorHide;
    QDeclarativeView* mDeclarativeView;
};

}
}

#endif //ANDROID
#endif // QMLAMLAUTHENTICATIONDIALOG_HPP
