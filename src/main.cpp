#include "AppTheme.h"
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
    a.setWindowIcon(AppTheme::appIcon());
    a.setStyleSheet(QString(R"(
        QMainWindow, QWidget {
            background: %1;
            color: %2;
        }
        QWidget#appHeaderBar {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 %3, stop:0.52 %3, stop:0.52 %4, stop:1 %4);
        }
        QLabel#appHeaderTitulo {
            color: #ffffff;
            font-size: 19px;
            font-weight: 800;
            letter-spacing: 0.3px;
            background: transparent;
        }
        QLabel#appHeaderSubtitulo {
            color: rgba(255, 255, 255, 0.85);
            font-size: 11px;
            font-weight: 600;
            background: transparent;
        }
        QLabel#appHeaderSelo {
            color: rgba(255, 255, 255, 0.75);
            font-size: 10px;
            font-weight: 700;
            background: transparent;
        }
        QGroupBox {
            background: %5;
            border: 1px solid %6;
            border-top: 3px solid %4;
            border-radius: 12px;
            margin-top: 16px;
            padding: 20px 16px 16px 16px;
            font-weight: 700;
            font-size: 11.5px;
            color: %7;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            top: 3px;
            padding: 0 8px;
            background: %5;
            color: %4;
        }
        QLabel {
            background: transparent;
            color: %7;
        }
        QLabel#lblMensagem[erro="false"] { color: %3; font-weight: 700; }
        QLineEdit, QComboBox, QDateEdit, QPlainTextEdit, QSpinBox {
            background: %5;
            border: 1.5px solid %6;
            border-radius: 8px;
            padding: 7px 10px;
            min-height: 24px;
            selection-background-color: %4;
            selection-color: #ffffff;
            color: %7;
        }
        QLineEdit:hover, QComboBox:hover, QDateEdit:hover, QPlainTextEdit:hover {
            border: 1.5px solid #b7c6d6;
        }
        QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QPlainTextEdit:focus {
            border: 2px solid %4;
        }
        QLineEdit:disabled, QComboBox:disabled, QDateEdit:disabled {
            background: #eef2f6;
            color: #96a4b3;
            border: 1.5px solid %6;
        }
        QComboBox::drop-down, QDateEdit::drop-down {
            border: 0;
            width: 26px;
        }
        QComboBox QAbstractItemView {
            background: %5;
            border: 1px solid %6;
            selection-background-color: %8;
            selection-color: %4;
            outline: 0;
            padding: 4px;
        }
        QPushButton {
            background: %8;
            color: %4;
            border: 1px solid transparent;
            border-radius: 9px;
            padding: 9px 17px;
            font-weight: 700;
            min-height: 24px;
        }
        QPushButton:hover { background: #d9e8f6; }
        QPushButton:pressed { background: #c3d9ee; }
        QPushButton:disabled { background: #eef2f6; color: #a3afba; }

        QPushButton#btnSalvar, QPushButton#btnAtualizar, QPushButton#btnConsultar,
        QPushButton#btnBuscar, QPushButton#btnBuscarPaciente, QPushButton#btnVerificarAcompanhante,
        QPushButton#btnAdicionar, QPushButton#btnListar, QPushButton#btnHistorico,
        QPushButton#btnVolume, QPushButton#btnMapa, QPushButton#btnExportarPlanilha,
        QPushButton#btnGerarPdf, QPushButton#btnExportarPlanilhaViagem,
        QPushButton#btnGerarPdfViagem {
            background: %4;
            color: #ffffff;
            border: 1px solid %9;
        }
        QPushButton#btnSalvar:hover, QPushButton#btnAtualizar:hover, QPushButton#btnConsultar:hover,
        QPushButton#btnBuscar:hover, QPushButton#btnBuscarPaciente:hover, QPushButton#btnVerificarAcompanhante:hover,
        QPushButton#btnAdicionar:hover, QPushButton#btnListar:hover, QPushButton#btnHistorico:hover,
        QPushButton#btnVolume:hover, QPushButton#btnMapa:hover, QPushButton#btnExportarPlanilha:hover,
        QPushButton#btnGerarPdf:hover, QPushButton#btnExportarPlanilhaViagem:hover,
        QPushButton#btnGerarPdfViagem:hover {
            background: %9;
        }
        QPushButton#btnSalvar:pressed, QPushButton#btnAtualizar:pressed, QPushButton#btnConsultar:pressed,
        QPushButton#btnBuscar:pressed, QPushButton#btnAdicionar:pressed, QPushButton#btnListar:pressed {
            background: %10;
        }
        QPushButton#btnDeletar, QPushButton#btnExcluir {
            background: %11;
            color: #ffffff;
            border: 1px solid %12;
        }
        QPushButton#btnDeletar:hover, QPushButton#btnExcluir:hover { background: %12; }
        QPushButton#btnVoltar {
            background: transparent;
            color: %4;
            border: 1px solid transparent;
            padding: 8px 14px;
        }
        QPushButton#btnVoltar:hover { background: %8; border: 1px solid %6; }
        QTableWidget {
            background: %5;
            alternate-background-color: %13;
            border: 1px solid %6;
            border-radius: 10px;
            gridline-color: #eef2f6;
            selection-background-color: %8;
            selection-color: %4;
        }
        QTableWidget::item { padding: 4px 2px; }
        QHeaderView::section {
            background: %14;
            color: %4;
            border: 0;
            border-bottom: 2px solid %4;
            padding: 10px 8px;
            font-weight: 800;
            font-size: 10.5px;
        }
        QTabWidget::pane {
            background: %5;
            border: 1px solid %6;
            border-radius: 10px;
            top: -1px;
        }
        QTabBar::tab {
            background: %14;
            color: %7;
            border: 0;
            border-radius: 8px 8px 0 0;
            padding: 10px 18px;
            margin-right: 4px;
            font-weight: 700;
        }
        QTabBar::tab:hover { background: %8; }
        QTabBar::tab:selected { background: %5; color: %4; border-top: 3px solid %3; }
        QStatusBar {
            background: %2;
            color: #ffffff;
        }
        QStatusBar QLabel { color: rgba(255, 255, 255, 0.82); font-weight: 600; }
        QScrollBar:vertical { background: #e7edf3; width: 11px; margin: 4px; border-radius: 5px; }
        QScrollBar::handle:vertical { background: #aebccb; border-radius: 5px; min-height: 28px; }
        QScrollBar::handle:vertical:hover { background: %4; }
        QScrollBar:horizontal { background: #e7edf3; height: 11px; margin: 4px; border-radius: 5px; }
        QScrollBar::handle:horizontal { background: #aebccb; border-radius: 5px; min-width: 28px; }
        QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }
        QToolTip {
            background: %2;
            color: #ffffff;
            border: 1px solid %4;
            padding: 6px 9px;
            border-radius: 6px;
        }
        QMessageBox { background: %5; }
    )")
                        .arg(AppTheme::FundoPagina)  // %1 fundo geral da janela
                        .arg(AppTheme::Tinta)        // %2 texto/status bar
                        .arg(AppTheme::Verde)        // %3 verde institucional
                        .arg(AppTheme::Azul)         // %4 azul institucional (cor primária)
                        .arg(AppTheme::Cartao)       // %5 branco dos cartões/campos
                        .arg(AppTheme::Borda)        // %6 borda neutra
                        .arg(AppTheme::Tinta)        // %7 texto padrão
                        .arg(AppTheme::AzulClaro)    // %8 azul claro (hover/seleção)
                        .arg(AppTheme::AzulEscuro)   // %9 azul escuro (hover botão primário)
                        .arg(AppTheme::AzulEscuro)   // %10 azul escuro (pressed)
                        .arg(AppTheme::Perigo)       // %11 vermelho (excluir)
                        .arg(AppTheme::PerigoEscuro) // %12 vermelho escuro (hover excluir)
                        .arg(AppTheme::FundoPagina)  // %13 linha alternada da tabela
                        .arg(AppTheme::AzulClaro));  // %14 cabeçalho de tabela/aba

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