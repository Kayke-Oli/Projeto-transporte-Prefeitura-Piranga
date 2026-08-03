#pragma once

#include <stdexcept>
#include <string>

// Centraliza a conversão entre o formato exibido pela aplicação
// ("DD-MM-YYYY") e o formato ISO que o PostgreSQL usa para DATE
// ("YYYY-MM-DD"). Além da forma, valida o calendário gregoriano: uma data
// impossível nunca deve chegar ao banco por uma chamada fora da interface.
namespace DataUtils
{
    namespace Detalhe
    {
        inline bool saoDigitos(const std::string &texto, std::size_t inicio, std::size_t quantidade)
        {
            for (std::size_t i = inicio; i < inicio + quantidade; ++i)
            {
                if (texto[i] < '0' || texto[i] > '9')
                    return false;
            }
            return true;
        }

        inline int numero(const std::string &texto, std::size_t inicio, std::size_t quantidade)
        {
            int valor = 0;
            for (std::size_t i = inicio; i < inicio + quantidade; ++i)
                valor = valor * 10 + (texto[i] - '0');
            return valor;
        }

        inline bool anoBissexto(int ano)
        {
            return (ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0;
        }

        inline bool dataValida(int dia, int mes, int ano)
        {
            if (ano < 1 || ano > 9999 || mes < 1 || mes > 12 || dia < 1)
                return false;

            static constexpr int diasPorMes[] = {
                31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            const int limite = mes == 2 && anoBissexto(ano) ? 29 : diasPorMes[mes - 1];
            return dia <= limite;
        }

        inline void exigirDataValida(int dia, int mes, int ano, const char *formato)
        {
            if (!dataValida(dia, mes, ano))
                throw std::invalid_argument(std::string("Data inválida (esperado ") + formato + ").");
        }
    }

    // "21-07-2026" -> "2026-07-21"
    inline std::string paraISO(const std::string &dataBR)
    {
        if (dataBR.size() != 10 || dataBR[2] != '-' || dataBR[5] != '-' ||
            !Detalhe::saoDigitos(dataBR, 0, 2) || !Detalhe::saoDigitos(dataBR, 3, 2) ||
            !Detalhe::saoDigitos(dataBR, 6, 4))
        {
            throw std::invalid_argument("Data inválida (esperado DD-MM-YYYY).");
        }

        const int dia = Detalhe::numero(dataBR, 0, 2);
        const int mes = Detalhe::numero(dataBR, 3, 2);
        const int ano = Detalhe::numero(dataBR, 6, 4);
        Detalhe::exigirDataValida(dia, mes, ano, "DD-MM-YYYY");

        return dataBR.substr(6, 4) + "-" + dataBR.substr(3, 2) + "-" + dataBR.substr(0, 2);
    }

    // "2026-07-21" -> "21-07-2026"
    inline std::string paraBR(const std::string &dataISO)
    {
        if (dataISO.size() != 10 || dataISO[4] != '-' || dataISO[7] != '-' ||
            !Detalhe::saoDigitos(dataISO, 0, 4) || !Detalhe::saoDigitos(dataISO, 5, 2) ||
            !Detalhe::saoDigitos(dataISO, 8, 2))
        {
            throw std::invalid_argument("Data inválida (esperado YYYY-MM-DD).");
        }

        const int ano = Detalhe::numero(dataISO, 0, 4);
        const int mes = Detalhe::numero(dataISO, 5, 2);
        const int dia = Detalhe::numero(dataISO, 8, 2);
        Detalhe::exigirDataValida(dia, mes, ano, "YYYY-MM-DD");

        return dataISO.substr(8, 2) + "-" + dataISO.substr(5, 2) + "-" + dataISO.substr(0, 4);
    }
}
