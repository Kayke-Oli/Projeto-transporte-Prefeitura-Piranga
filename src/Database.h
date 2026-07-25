#pragma once
#include <pqxx/pqxx>
#include <memory>
#include <iostream>
#include <string>
#include <cstdlib>
#include <stdexcept>

class Database
{
private:
    std::unique_ptr<pqxx::connection> conn;
    std::string connection_string;

    // Monta a connection string a partir de variáveis de ambiente,
    // evitando credenciais expostas no código-fonte / repositório Git.
    // Configure as variáveis antes de rodar o programa, por exemplo:
    //   DB_HOST, DB_PORT, DB_NAME, DB_USER, DB_PASSWORD, DB_SSLMODE
    static std::string montarConnStringDoAmbiente()
    {
        auto getEnvOrDefault = [](const char *nome, const std::string &padrao) -> std::string
        {
            const char *valor = std::getenv(nome);
            return valor ? std::string(valor) : padrao;
        };

        std::string host = getEnvOrDefault("DB_HOST", "127.0.0.1");
        std::string port = getEnvOrDefault("DB_PORT", "5432");
        std::string dbname = getEnvOrDefault("DB_NAME", "prefeitura_viagens");
        std::string user = getEnvOrDefault("DB_USER", "postgres");
        std::string password = getEnvOrDefault("DB_PASSWORD", "");
        std::string sslmode = getEnvOrDefault("DB_SSLMODE", "prefer");

        if (password.empty())
        {
            std::cerr << "Aviso: DB_PASSWORD nao definida no ambiente. "
                         "Configure as variaveis de ambiente DB_HOST, DB_PORT, DB_NAME, "
                         "DB_USER, DB_PASSWORD antes de conectar."
                      << std::endl;
        }

        return "dbname=" + dbname +
               " user=" + user +
               " password=" + password +
               " host=" + host +
               " port=" + port +
               " sslmode=" + sslmode;
    }

public:
    // Se nenhuma string for passada, monta a conexão a partir de variáveis de ambiente.
    // Assim, cada máquina/servidor pode ter sua própria configuração sem alterar o código.
    Database(const std::string &conn_str = "")
        : connection_string(conn_str.empty() ? montarConnStringDoAmbiente() : conn_str) {}

    void conectar()
    {
        try
        {
            conn = std::make_unique<pqxx::connection>(connection_string);
            if (conn->is_open())
            {
                std::cout << "Banco de dados conectado com sucesso!" << std::endl;
            }
            else
            {
                std::cerr << "Falha ao conectar no banco de dados." << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Erro de conexão: " << e.what() << std::endl;
        }
    }

    // Garante que a conexão está ativa antes de usar; tenta reconectar se necessário.
    // Importante em ambiente multi-máquina, onde quedas de rede são mais prováveis
    // do que em uso puramente local.
    //
    // is_open() sozinho NÃO é suficiente: ele só reflete o estado local do
    // socket, não sabe que o servidor do outro lado caiu até a conexão ser
    // efetivamente usada. Por isso, além de is_open(), fazemos um ping ativo
    // (uma query trivial) para confirmar que a conexão realmente responde
    // antes de reaproveitá-la.
    bool garantirConectado()
    {
        try
        {
            if (conn && conn->is_open())
            {
                try
                {
                    pqxx::nontransaction ping(*conn);
                    ping.exec("SELECT 1");
                    return true; // conexão confirmada viva
                }
                catch (const std::exception &)
                {
                    // is_open() dizia que sim, mas a conexão está morta de
                    // verdade (ex.: servidor caiu). Cai para reconectar abaixo.
                }
            }
            conn = std::make_unique<pqxx::connection>(connection_string);
            return conn->is_open();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Erro ao (re)conectar: " << e.what() << std::endl;
            return false;
        }
    }

    // Garante que a conexão está pronta para uso; tenta reconectar se
    // necessário (via garantirConectado()) e lança uma exceção clara se
    // não conseguir. Pensado para ser chamado como a primeira linha de
    // cada método de repository, dentro do try, antes de abrir uma
    // transação — assim, o mesmo catch que já existe em cada método
    // também cobre falhas de (re)conexão, sem precisar de tratamento
    // especial em cada lugar.
    void exigirConexao()
    {
        if (!garantirConectado())
        {
            throw std::runtime_error(
                "Não foi possível conectar ao banco de dados (conexão perdida "
                "e tentativa de reconexão falhou).");
        }
    }

    void desconectar()
    {
        // Em versões recentes da libpqxx (7.x), pqxx::connection não tem
        // mais o método disconnect(). O jeito correto de fechar a conexão
        // é simplesmente destruir o objeto — o unique_ptr faz isso por nós
        // (RAII) ao chamarmos reset(), que libera o ponteiro atual antes
        // de assumir nullptr.
        if (conn)
        {
            conn.reset();
        }
    }

    pqxx::connection *getConexao()
    {
        return conn.get();
    }
};