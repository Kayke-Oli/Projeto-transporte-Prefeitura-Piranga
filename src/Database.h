#pragma once

#include <cstdlib>
#include <memory>
#include <pqxx/pqxx>
#include <set>
#include <stdexcept>
#include <string>

class Database
{
private:
    std::unique_ptr<pqxx::connection> conn;
    std::string connection_string;

    static bool sslModeGarantidamenteEncriptado(const std::string &sslmode)
    {
        static const std::set<std::string> modosSeguros = {"require", "verify-ca", "verify-full"};
        return modosSeguros.count(sslmode) != 0;
    }

    // Valores de conninfo precisam de aspas e escape próprio do libpq. Isso
    // permite senhas com espaços/apóstrofos sem expor ou interpolar parâmetros.
    static std::string valorConninfo(const std::string &valor)
    {
        std::string resultado;
        resultado.reserve(valor.size() + 2);
        resultado += '\'';
        for (const char caractere : valor)
        {
            if (caractere == '\\' || caractere == '\'')
                resultado += '\\';
            resultado += caractere;
        }
        resultado += '\'';
        return resultado;
    }

    static void adicionarParametro(std::string &conninfo, const char *nome, const std::string &valor)
    {
        if (!conninfo.empty())
            conninfo += ' ';
        conninfo += nome;
        conninfo += '=';
        conninfo += valorConninfo(valor);
    }

    static std::string montarConnStringDoAmbiente()
    {
        const auto getEnvOrDefault = [](const char *nome, const std::string &padrao)
        {
            const char *valor = std::getenv(nome);
            return valor != nullptr ? std::string(valor) : padrao;
        };

        const std::string host = getEnvOrDefault("DB_HOST", "127.0.0.1");
        const std::string port = getEnvOrDefault("DB_PORT", "5432");
        const std::string dbname = getEnvOrDefault("DB_NAME", "prefeitura_viagens");
        const std::string user = getEnvOrDefault("DB_USER", "postgres");
        const std::string password = getEnvOrDefault("DB_PASSWORD", "");
        const std::string sslmode = getEnvOrDefault("DB_SSLMODE", "require");
        const std::string sslrootcert = getEnvOrDefault("DB_SSLROOTCERT", "");

        if (password.empty())
        {
            throw std::runtime_error(
                "DB_PASSWORD não foi definida. Configure as credenciais do PostgreSQL no ambiente antes de iniciar o sistema.");
        }

        if (!sslModeGarantidamenteEncriptado(sslmode))
        {
            throw std::runtime_error(
                "DB_SSLMODE='" + sslmode +
                "' não garante conexão criptografada. Use 'require', 'verify-ca' ou 'verify-full'.");
        }

        if ((sslmode == "verify-ca" || sslmode == "verify-full") && sslrootcert.empty())
        {
            throw std::runtime_error(
                "DB_SSLMODE='" + sslmode +
                "' exige DB_SSLROOTCERT apontando para o certificado da autoridade certificadora.");
        }

        if (port.empty() || port.find_first_not_of("0123456789") != std::string::npos)
            throw std::runtime_error("DB_PORT deve conter apenas números.");

        const unsigned long portaNumerica = std::stoul(port);
        if (portaNumerica == 0 || portaNumerica > 65535)
            throw std::runtime_error("DB_PORT deve estar entre 1 e 65535.");

        std::string conninfo;
        adicionarParametro(conninfo, "dbname", dbname);
        adicionarParametro(conninfo, "user", user);
        adicionarParametro(conninfo, "password", password);
        adicionarParametro(conninfo, "host", host);
        adicionarParametro(conninfo, "port", port);
        adicionarParametro(conninfo, "sslmode", sslmode);
        if (!sslrootcert.empty())
            adicionarParametro(conninfo, "sslrootcert", sslrootcert);
        return conninfo;
    }

    void abrirConexao()
    {
        conn = std::make_unique<pqxx::connection>(connection_string);
        if (!conn->is_open())
            throw std::runtime_error("O PostgreSQL recusou a abertura da conexão.");
    }

public:
    explicit Database(const std::string &connStr = "")
        : connection_string(connStr.empty() ? montarConnStringDoAmbiente() : connStr) {}

    // Não captura exceções: a inicialização deve falhar de forma explícita e
    // a interface decide como comunicar a indisponibilidade do banco.
    void conectar()
    {
        abrirConexao();
    }

    void exigirConexao()
    {
        if (conn && conn->is_open())
        {
            try
            {
                pqxx::nontransaction ping(*conn);
                ping.exec("SELECT 1");
                return;
            }
            catch (const pqxx::broken_connection &)
            {
                conn.reset();
            }
        }

        // A exceção original de reconexão segue até o Repository e, daí, à UI.
        abrirConexao();
    }

    void desconectar()
    {
        conn.reset();
    }

    pqxx::connection *getConexao()
    {
        return conn.get();
    }
};
