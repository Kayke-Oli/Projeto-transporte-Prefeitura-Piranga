#pragma once

#include <QFrame>

// QPushButton não foi projetado para conter um QLayout com widgets reais:
// o Qt calcula a área de conteúdo do botão a partir de texto/ícone, não do
// layout, então o conteúdo pode ficar cortado ou distorcido dependendo do
// estilo/DPI. ClickableCard resolve isso sendo um QFrame comum (sem essa
// limitação) que apenas emite clicked() ao ser pressionado e solto dentro
// dos seus limites — mesmo comportamento de clique de um botão.
class ClickableCard : public QFrame
{
    Q_OBJECT
public:
    explicit ClickableCard(QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *evento) override;
    void mouseReleaseEvent(QMouseEvent *evento) override;

private:
    bool m_pressionado = false;
};