#pragma once
#include <string>
#include <algorithm>

// Segue o mesmo padrão do DataUtils.h: uma função utilitária pura,
// sem estado e sem depender do banco, isolada num namespace próprio.
//
// Por que não colocar isso como método dentro da struct Paciente?
// Paciente (em Entidades.h) é modelada como um DTO/entidade de domínio
// "burra" - só dados, sem comportamento. Validação de CPF não é uma
// característica exclusiva de paciente: Acompanhante e Motorista também
// têm CPF e precisariam da mesma validação. Colocando a função aqui,
// qualquer um dos três pode reutilizá-la sem duplicar código nem
// "inchar" a struct Paciente com lógica que não é dela.
namespace CpfUtils
{
    // Espera uma string com exatamente 11 dígitos numéricos, sem pontuação
    // (ex.: "11144477735", não "111.444.777-35"). Se seu formulário aceita
    // CPF formatado, remova pontos e traço antes de chamar esta função.
    inline bool verificar(const std::string &cpf)
    {
        if (cpf.length() != 11)
        {
            return false;
        }

        if (!std::all_of(cpf.begin(), cpf.end(), ::isdigit))
        {
            return false;
        }

        // CPFs com todos os dígitos iguais (111.111.111-11, 000.000.000-00,
        // etc.) passam matematicamente no cálculo abaixo, mas nunca são
        // emitidos pela Receita Federal - precisam ser rejeitados à parte.
        if (std::all_of(cpf.begin(), cpf.end(), [&](char c)
                        { return c == cpf[0]; }))
        {
            return false;
        }

        // --- 1o dígito verificador: usa os 9 primeiros dígitos, pesos 10..2 ---
        int soma = 0;
        int peso = 10;
        for (int i = 0; i < 9; i++)
        {
            soma += (cpf[i] - '0') * peso;
            peso--;
        }
        int resto = soma % 11;
        int digito1 = (resto < 2) ? 0 : (11 - resto);
        if ((cpf[9] - '0') != digito1)
        {
            return false;
        }

        // --- 2o dígito verificador: usa os 10 primeiros dígitos, pesos 11..2 ---
        soma = 0;
        peso = 11;
        for (int i = 0; i < 10; i++)
        {
            soma += (cpf[i] - '0') * peso;
            peso--;
        }
        resto = soma % 11;
        int digito2 = (resto < 2) ? 0 : (11 - resto);
        if ((cpf[10] - '0') != digito2)
        {
            return false;
        }

        return true;
    }
}