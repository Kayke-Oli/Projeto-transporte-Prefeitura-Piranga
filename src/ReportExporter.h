#pragma once

#include "Entidades.h"

#include <QString>

class ReportExporter
{
public:
    static bool exportarCsv(const RelatorioViagem &relatorio, const QString &caminho, QString *erro = nullptr);
    static bool exportarPdf(const RelatorioViagem &relatorio, const QString &caminho, QString *erro = nullptr);
};
