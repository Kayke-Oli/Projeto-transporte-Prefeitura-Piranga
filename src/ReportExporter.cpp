#include "ReportExporter.h"

#include <QFileInfo>
#include <QMarginsF>
#include <QPageLayout>
#include <QPageSize>
#include <QPdfWriter>
#include <QSaveFile>
#include <QStringConverter>
#include <QTextDocument>
#include <QTextStream>
#include <QVector>

namespace
{
    QString somenteDigitos(const QString &texto)
    {
        QString resultado;
        resultado.reserve(texto.size());
        for (const QChar caractere : texto)
        {
            if (caractere.isDigit())
                resultado += caractere;
        }
        return resultado;
    }

    QString formatarCpf(const std::string &cpf)
    {
        const QString digitos = somenteDigitos(QString::fromStdString(cpf));
        if (digitos.size() != 11)
            return digitos;
        return digitos.left(3) + "." + digitos.mid(3, 3) + "." + digitos.mid(6, 3) + "-" + digitos.right(2);
    }

    QString formatarTelefone(const std::string &telefone)
    {
        const QString digitos = somenteDigitos(QString::fromStdString(telefone));
        if (digitos.size() == 11)
            return "(" + digitos.left(2) + ") " + digitos.mid(2, 5) + "-" + digitos.right(4);
        if (digitos.size() == 10)
            return "(" + digitos.left(2) + ") " + digitos.mid(2, 4) + "-" + digitos.right(4);
        return digitos;
    }

    QString textoAcompanhante(const RelatorioViagemPassageiro &passageiro)
    {
        if (!passageiro.acompanhanteNome.has_value())
            return {};

        QString texto = QString::fromStdString(*passageiro.acompanhanteNome);
        if (passageiro.acompanhanteTelefone.has_value() && !passageiro.acompanhanteTelefone->empty())
            texto += " - " + formatarTelefone(*passageiro.acompanhanteTelefone);
        return texto;
    }

    QString csvCampo(QString texto)
    {
        texto.replace('"', "\"\"");
        return '"' + texto + '"';
    }

    QVector<QString> dadosDoPassageiro(const RelatorioViagemPassageiro &passageiro)
    {
        return {QString::fromStdString(passageiro.nome), formatarCpf(passageiro.cpf),
                formatarTelefone(passageiro.telefone), textoAcompanhante(passageiro)};
    }

    QString htmlSeguro(const QString &texto)
    {
        return texto.toHtmlEscaped();
    }
}

bool ReportExporter::exportarCsv(const RelatorioViagem &relatorio, const QString &caminho, QString *erro)
{
    QSaveFile arquivo(caminho);
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if (erro)
            *erro = "Não foi possível criar a planilha: " + arquivo.errorString();
        return false;
    }

    arquivo.write("\xEF\xBB\xBF");
    QTextStream saida(&arquivo);
    saida.setEncoding(QStringConverter::Utf8);
    saida << "Nome;CPF;Telefone;Acompanhante e telefone\n";
    for (const auto &passageiro : relatorio.passageiros)
    {
        const QVector<QString> dados = dadosDoPassageiro(passageiro);
        saida << csvCampo(dados[0]) << ';' << csvCampo(dados[1]) << ';' << csvCampo(dados[2]) << ';'
              << csvCampo(dados[3]) << '\n';
    }
    saida.flush();

    if (!arquivo.commit())
    {
        if (erro)
            *erro = "Não foi possível finalizar a planilha: " + arquivo.errorString();
        return false;
    }
    return true;
}

bool ReportExporter::exportarPdf(const RelatorioViagem &relatorio, const QString &caminho, QString *erro)
{
    QPdfWriter escritor(caminho);
    escritor.setResolution(144);
    escritor.setPageSize(QPageSize(QPageSize::A4));
    escritor.setPageOrientation(QPageLayout::Landscape);
    escritor.setPageMargins(QMarginsF(10, 10, 10, 10));
    escritor.setTitle("Relatório de passageiros");

    QString equipe = "Motorista: " + QString::fromStdString(relatorio.motorista);
    if (relatorio.auxiliar.has_value())
        equipe += " &nbsp;&nbsp;&nbsp; Auxiliar: " + htmlSeguro(QString::fromStdString(*relatorio.auxiliar));

    QString html = R"(
        <html><head><style>
        body { color: #1d2939; font-family: Arial, sans-serif; font-size: 10pt; margin: 48px; }
        h1 { color: #102a43; font-size: 20pt; margin: 0 0 10px 0; }
        p.info { margin: 2px 0; }
        table { border-collapse: collapse; margin-top: 16px; width: 100%; }
        thead { display: table-header-group; }
        tr { page-break-inside: avoid; }
        th { background-color: #eaf1f8; color: #243b53; font-weight: bold; text-align: left; }
        th, td { border: 1px solid #9fb3c8; padding: 7px; vertical-align: top; }
        </style></head><body>)";

    html += "<h1>Relatório de passageiros - Viagem " + QString::number(relatorio.viagemId) + "</h1>";
    html += "<p class=\"info\">Data: " + htmlSeguro(QString::fromStdString(relatorio.dataViagem)) +
            " &nbsp;&nbsp;&nbsp; Destino: " + htmlSeguro(QString::fromStdString(relatorio.cidadeDestino)) +
            " &nbsp;&nbsp;&nbsp; Veículo: " + htmlSeguro(QString::fromStdString(relatorio.veiculoPlaca)) + " - " +
            htmlSeguro(QString::fromStdString(relatorio.veiculoModelo)) + "</p>";
    html += "<p class=\"info\">" + equipe + "</p>";
    html += "<table><thead><tr><th>Nome</th><th>CPF</th><th>Telefone</th><th>Acompanhante e telefone</th></tr></thead><tbody>";
    for (const auto &passageiro : relatorio.passageiros)
    {
        const QVector<QString> dados = dadosDoPassageiro(passageiro);
        html += "<tr><td>" + htmlSeguro(dados[0]) + "</td><td>" + htmlSeguro(dados[1]) + "</td><td>" +
                htmlSeguro(dados[2]) + "</td><td>" + htmlSeguro(dados[3]) + "</td></tr>";
    }
    html += "</tbody></table>";
    if (relatorio.passageiros.empty())
        html += "<p>Nenhum paciente foi registrado nesta viagem.</p>";
    html += "</body></html>";

    QTextDocument documento;
    documento.setDocumentMargin(0);
    documento.setPageSize(QSizeF(escritor.width(), escritor.height()));
    documento.setHtml(html);
    documento.print(&escritor);

    const QFileInfo arquivo(caminho);
    if (!arquivo.exists() || arquivo.size() == 0)
    {
        if (erro)
            *erro = "O PDF não foi criado corretamente.";
        return false;
    }
    return true;
}
