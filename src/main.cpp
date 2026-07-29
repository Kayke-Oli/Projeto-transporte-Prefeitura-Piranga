#include "Database.h"
#include "MainWindow.h"
#include <QApplication>
#include <QMessageBox>
#include <memory>

int main(int argc, char *argv[])
{
    // Inicializa o motor gráfico do Qt
    QApplication a(argc, argv);

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

    db->conectar();

    // Instancia e exibe a tela principal que você construiu
    MainWindow w(*db);
    w.show();

    // Entrega o controle do programa para o loop de eventos do Qt
    int resultado = a.exec();

    db->desconectar();
    return resultado;
}