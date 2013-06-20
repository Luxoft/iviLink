#include <QLabel>
#include <QMovie>
#include <QMouseEvent>
#include <QGraphicsBlurEffect>

#include "ivilinkprogressbar.hpp"

static const char* shutdown = "../applications/shutdown.sh";

IVILinkProgressBar::IVILinkProgressBar(QWidget *parent)
    : QWidget(parent)
{
    mMovie = NULL;
    static const int windowWidth = 800;
    static const int windowHeight = 600;

    adjustSize();
    setFixedSize(windowWidth, windowHeight);

    QImage backgroundImage = QImage(":img/Ai.png");
    QLabel * background = new QLabel(this);
    background->setPixmap(QPixmap::fromImage(backgroundImage)); 
    QGraphicsBlurEffect *effect = new QGraphicsBlurEffect(this);
    background->setGraphicsEffect(effect);

    QLabel *label = new QLabel(this);
    mMovie = new QMovie(":img/progressBar.gif");
    label->setMovie(mMovie);
    mMovie->start();
    label->move(60, 527);
}

void IVILinkProgressBar::mousePressEvent(QMouseEvent * event)
{
    if (QRect(0, 0, 50, 50).contains(event->pos() ) )
        system(shutdown);
}

IVILinkProgressBar::~IVILinkProgressBar()
{
    delete mMovie;
}
