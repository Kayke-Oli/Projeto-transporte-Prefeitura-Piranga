#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Database.h"

// Declaração antecipada das views (telas)
class ViewPacientes;
class ViewViagens;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Database &db, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void alterarTela(int index);

private:
    Database &m_db;

    // Componentes de Layout
    QWidget *m_centralWidget;
    QHBoxLayout *m_mainLayout;
    QListWidget *m_menuLateral;
    QStackedWidget *m_stackTelas;

    // Instâncias das Telas
    ViewPacientes *m_telaPacientes;
    ViewViagens *m_telaViagens;

    void configurarInterface();
    void configurarEstilo();
};

#endif