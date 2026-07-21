#pragma once
#include <string>
#include <stdexcept>

// Centraliza a conversão entre o formato de data usado no restante do
// sistema ("DD-MM-YYYY", ver Entidades.h) e o formato ISO ("YYYY-MM-DD")
// que o tipo DATE do PostgreSQL espera na entrada e devolve na saída.
//
// Só o ViagemRepository deveria conhecer este arquivo: é a "borda" entre
// a representação de data do domínio (C++) e a representação do banco.
namespace DataUtils
{
    // "21-07-2026" -> "2026-07-21"
    inline std::string paraISO(const std::string &dataBR)
    {
        if (dataBR.size() != 10 || dataBR[2] != '-' || dataBR[5] != '-')
        {
            throw std::invalid_argument(
                "Data em formato invalido (esperado DD-MM-YYYY): " + dataBR);
        }

        std::string dia = dataBR.substr(0, 2);
        std::string mes = dataBR.substr(3, 2);
        std::string ano = dataBR.substr(6, 4);

        return ano + "-" + mes + "-" + dia;
    }

    // "2026-07-21" -> "21-07-2026"
    // Usado ao ler de volta valores DATE que o PostgreSQL devolve em ISO.
    inline std::string paraBR(const std::string &dataISO)
    {
        if (dataISO.size() != 10 || dataISO[4] != '-' || dataISO[7] != '-')
        {
            throw std::invalid_argument(
                "Data em formato invalido (esperado YYYY-MM-DD): " + dataISO);
        }

        std::string ano = dataISO.substr(0, 4);
        std::string mes = dataISO.substr(5, 2);
        std::string dia = dataISO.substr(8, 2);

        return dia + "-" + mes + "-" + ano;
    }
}