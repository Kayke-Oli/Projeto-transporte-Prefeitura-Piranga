#include "CpfUtils.h"
#include "DataUtils.h"

#include <exception>
#include <iostream>
#include <string>

namespace
{
    int falhas = 0;

    void verificar(bool condicao, const std::string &mensagem)
    {
        if (!condicao)
        {
            std::cerr << "FALHA: " << mensagem << '\n';
            ++falhas;
        }
    }

    template <typename Funcao>
    void verificarExcecao(Funcao &&funcao, const std::string &mensagem)
    {
        try
        {
            funcao();
            verificar(false, mensagem);
        }
        catch (const std::invalid_argument &)
        {
        }
        catch (const std::exception &)
        {
            verificar(false, mensagem + " (tipo de exceção inesperado)");
        }
    }
}

int main()
{
    verificar(CpfUtils::normalizar("529.982.247-25") == "52998224725", "CPF deve ser normalizado");
    verificar(CpfUtils::verificar("529.982.247-25"), "CPF conhecido válido deve ser aceito");
    verificar(CpfUtils::verificar("111.444.777-35"), "CPF conhecido válido com máscara deve ser aceito");
    verificar(!CpfUtils::verificar("111.111.111-11"), "CPF com dígitos repetidos deve ser rejeitado");
    verificar(!CpfUtils::verificar("529.982.247-26"), "CPF com dígito verificador incorreto deve ser rejeitado");
    verificar(!CpfUtils::verificar("529.982.247"), "CPF incompleto deve ser rejeitado");

    verificar(DataUtils::paraISO("29-02-2024") == "2024-02-29", "ano bissexto deve ser convertido");
    verificar(DataUtils::paraBR("2024-02-29") == "29-02-2024", "data ISO válida deve ser convertida");
    verificarExcecao([] { DataUtils::paraISO("29-02-2023"); }, "29 de fevereiro fora de ano bissexto deve falhar");
    verificarExcecao([] { DataUtils::paraISO("31-04-2026"); }, "dia inexistente deve falhar");
    verificarExcecao([] { DataUtils::paraISO("aa-01-2026"); }, "data com letras deve falhar");
    verificarExcecao([] { DataUtils::paraBR("2026-13-01"); }, "mês ISO inválido deve falhar");

    if (falhas != 0)
    {
        std::cerr << falhas << " teste(s) falharam.\n";
        return 1;
    }

    std::cout << "Todos os testes de CPF e data passaram.\n";
    return 0;
}
