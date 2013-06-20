#ifndef IVILINKPROGRESSBAR_H
#define IVILINKPROGRESSBAR_H

#include <QWidget>

class QMovie;

class IVILinkProgressBar : public QWidget
{
    Q_OBJECT

public:
    IVILinkProgressBar(QWidget *parent = 0);
    ~IVILinkProgressBar();

private:
    void mousePressEvent(QMouseEvent * event);
    QMovie * mMovie;
};

#endif // IVILINKPROGRESSBAR_H
