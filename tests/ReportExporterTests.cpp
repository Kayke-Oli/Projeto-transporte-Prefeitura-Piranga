#include "ReportExporter.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>

#include <cstdlib>
#include <iostream>

namespace
{
    void exigir(bool condicao, const char *mensagem)
    {
        if (!condicao)
        {
            std::cerr << mensagem << '\n';
            std::exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTemporaryDir diretorio;
    exigir(diretorio.isValid(), "Nao foi possivel criar o diretorio temporario.");

    RelatorioViagem relatorio;
    relatorio.viagemId = 42;
    relatorio.dataViagem = "04-08-2026";
    relatorio.cidadeDestino = "Conselheiro Lafaiete";
    relatorio.veiculoPlaca = "ABC1D23";
    relatorio.veiculoModelo = "Van";
    relatorio.motorista = "Maria Silva";
    relatorio.auxiliar = "Joao Souza";
    relatorio.passageiros.push_back({"Ana Maria", "12345678909", "31999998888", std::string("Carlos"),
                                     std::string("31988887777")});
    relatorio.passageiros.push_back({"Bruno Alves", "98765432100", "", std::nullopt, std::nullopt});
    for (int indice = 0; indice < 60; ++indice)
    {
        RelatorioViagemPassageiro passageiro;
        passageiro.nome = "Paciente de teste " + std::to_string(indice + 1);
        passageiro.cpf = "12345678909";
        passageiro.telefone = "31999998888";
        if (indice % 2 == 0)
        {
            passageiro.acompanhanteNome = "Acompanhante de teste com nome longo";
            passageiro.acompanhanteTelefone = "31988887777";
        }
        relatorio.passageiros.push_back(passageiro);
    }

    QString erro;
    const QString csv = diretorio.filePath("viagem.csv");
    exigir(ReportExporter::exportarCsv(relatorio, csv, &erro), "A exportacao CSV falhou.");
    QFile arquivoCsv(csv);
    exigir(arquivoCsv.open(QIODevice::ReadOnly), "Nao foi possivel ler o CSV.");
    const QByteArray conteudoCsv = arquivoCsv.readAll();
    exigir(conteudoCsv.contains("Nome;CPF;Telefone;Acompanhante e telefone"), "Cabecalho CSV incorreto.");
    exigir(conteudoCsv.contains("Ana Maria"), "Paciente ausente no CSV.");
    exigir(conteudoCsv.contains("Carlos"), "Acompanhante ausente no CSV.");

    const QString pdf = diretorio.filePath("viagem.pdf");
    exigir(ReportExporter::exportarPdf(relatorio, pdf, &erro), "A exportacao PDF falhou.");
    const QFileInfo arquivoPdf(pdf);
    exigir(arquivoPdf.exists() && arquivoPdf.size() > 0, "PDF nao foi criado.");
    if (qEnvironmentVariableIsSet("REPORT_EXPORTER_KEEP_OUTPUT"))
    {
        diretorio.setAutoRemove(false);
        qInfo().noquote() << "Arquivo de verificacao:" << pdf;
    }
    return 0;
}
