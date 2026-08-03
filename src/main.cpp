#include "Database.h"
#include "MainWindow.h"
#include <QApplication>
#include <QFont>
#include <QMessageBox>
#include <QStyleFactory>
#include <memory>

int main(int argc, char *argv[])
{
    // Inicializa o motor gráfico do Qt
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));
    a.setFont(QFont("Segoe UI", 10));
    a.setStyleSheet(R"(
        QMainWindow, QWidget {
            background: #f5f7fb;
            color: #1d2939;
        }
        QGroupBox {
            background: #ffffff;
            border: 1px solid #d9e2ec;
            border-radius: 12px;
            margin-top: 14px;
            padding: 18px 16px 14px 16px;
            font-weight: 700;
            color: #243b53;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 6px;
        }
        QLabel {
            background: transparent;
        }
        QLineEdit, QComboBox, QDateEdit, QPlainTextEdit {
            background: #ffffff;
            border: 1px solid #9fb3c8;
            border-radius: 7px;
            padding: 7px 9px;
            min-height: 24px;
            selection-background-color: #2e6fbb;
        }
        QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QPlainTextEdit:focus {
            border: 2px solid #2e6fbb;
        }
        QLineEdit:disabled, QComboBox:disabled, QDateEdit:disabled {
            background: #eef2f6;
            color: #7b8794;
        }
        QComboBox::drop-down, QDateEdit::drop-down {
            border: 0;
            width: 28px;
        }
        QPushButton {
            background: #e7eef6;
            color: #243b53;
            border: 0;
            border-radius: 8px;
            padding: 8px 15px;
            font-weight: 700;
            min-height: 24px;
        }
        QPushButton:hover { background: #d5e3f1; }
        QPushButton:pressed { background: #bfd3e6; }
        QPushButton:disabled { background: #eef2f6; color: #98a2b3; }
        QPushButton#btnSalvar, QPushButton#btnAtualizar, QPushButton#btnConsultar,
        QPushButton#btnBuscar, QPushButton#btnBuscarPaciente, QPushButton#btnBuscarAcompanhante,
        QPushButton#btnAdicionar, QPushButton#btnListar, QPushButton#btnHistorico,
        QPushButton#btnVolume, QPushButton#btnMapa {
            background: #1d5fa7;
            color: #ffffff;
        }
        QPushButton#btnSalvar:hover, QPushButton#btnAtualizar:hover, QPushButton#btnConsultar:hover,
        QPushButton#btnBuscar:hover, QPushButton#btnBuscarPaciente:hover, QPushButton#btnBuscarAcompanhante:hover,
        QPushButton#btnAdicionar:hover, QPushButton#btnListar:hover, QPushButton#btnHistorico:hover,
        QPushButton#btnVolume:hover, QPushButton#btnMapa:hover {
            background: #164c85;
        }
        QPushButton#btnDeletar, QPushButton#btnExcluir {
            background: #b42318;
            color: #ffffff;
        }
        QPushButton#btnDeletar:hover, QPushButton#btnExcluir:hover { background: #8a1c1c; }
        QPushButton#btnVoltar {
            background: transparent;
            color: #1d5fa7;
        }
        QPushButton#btnVoltar:hover { background: #e7eef6; }
        QTableWidget {
            background: #ffffff;
            alternate-background-color: #f8fafc;
            border: 1px solid #d9e2ec;
            border-radius: 9px;
            gridline-color: #edf2f7;
            selection-background-color: #dbeafe;
            selection-color: #102a43;
        }
        QHeaderView::section {
            background: #eaf1f8;
            color: #243b53;
            border: 0;
            border-bottom: 1px solid #d9e2ec;
            padding: 9px;
            font-weight: 700;
        }
        QTabWidget::pane {
            background: #ffffff;
            border: 1px solid #d9e2ec;
            border-radius: 9px;
            top: -1px;
        }
        QTabBar::tab {
            background: #e7eef6;
            color: #486581;
            border: 0;
            border-radius: 7px 7px 0 0;
            padding: 9px 16px;
            margin-right: 3px;
            font-weight: 700;
        }
        QTabBar::tab:selected { background: #ffffff; color: #1d5fa7; }
        QStatusBar { background: #102a43; color: #ffffff; }
        QScrollBar:vertical { background: #eef2f6; width: 10px; margin: 4px; }
        QScrollBar::handle:vertical { background: #9fb3c8; border-radius: 5px; min-height: 26px; }
        QToolTip { background: #102a43; color: #ffffff; border: 0; padding: 6px; }
    )");

    std::unique_ptr<Database> db;
    try
    {
        db = std::make_unique<Database>();
    }
    catch (const std::exception &e)
    {
        // Agora os erros fatais aparecem em janelas pop-up, não no terminal
        QMessageBox::critical(nullptr, "Erro de Configuração", e.what());
        return 1;
    }

    try
    {
        db->conectar();
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(nullptr, "Banco indisponível",
                              QString("Não foi possível conectar ao PostgreSQL.\n\n%1").arg(e.what()));
        return 1;
    }

    // Instancia e exibe a tela principal
    MainWindow w(*db);
    w.show();

    // Entrega o controle do programa para o loop de eventos do Qt
    int resultado = a.exec();

    db->desconectar();
    return resultado;
}
