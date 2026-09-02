#include "ClickableCard.h"
#include <QMouseEvent>

ClickableCard::ClickableCard(QWidget *parent) : QFrame(parent)
{
    setCursor(Qt::PointingHandCursor);
}

void ClickableCard::mousePressEvent(QMouseEvent *evento)
{
    if (evento->button() == Qt::LeftButton)
        m_pressionado = true;
    QFrame::mousePressEvent(evento);
}

void ClickableCard::mouseReleaseEvent(QMouseEvent *evento)
{
    if (evento->button() == Qt::LeftButton && m_pressionado && rect().contains(evento->pos()))
        emit clicked();
    m_pressionado = false;
    QFrame::mouseReleaseEvent(evento);
}