#pragma once
#include <QWidget>
#include "ViewPacientes.h" // para o enum ModoPaciente

class ViewPacientesMenu : public QWidget
{
    Q_OBJECT
public:
    explicit ViewPacientesMenu(QWidget *parent = nullptr);

signals:
    void opcaoEscolhida(ModoPaciente modo);
    void voltarSolicitado();

private:
    void configurarInterface();
};