# Sistema de Logística — Prefeitura de Piranga

Aplicação Qt 6/C++17 para o TFD (Tratamento Fora de Domicílio), com PostgreSQL e libpqxx.

## Recursos entregues

- Cadastro, consulta, atualização e exclusão protegida de pacientes.
- Cadastros de acompanhantes, motoristas e veículos.
- Planejamento completo de viagens: data, destino, veículo, motorista, pacientes e no máximo um acompanhante por paciente.
- Carregamento, alteração, listagem por data e exclusão de viagens.
- Relatórios de histórico do paciente, volume de pessoas e mapa diário de viagens.
- Validações de CPF, telefone, placa, datas e integridade referencial no PostgreSQL.

## Banco de dados

Para uma base nova:

```powershell
createdb prefeitura_viagens
psql -d prefeitura_viagens -f src/schema.sql
```

Para uma base existente, faça backup e execute, nesta ordem:

```powershell
psql -d prefeitura_viagens -f src/migracao_pacientes.sql
psql -d prefeitura_viagens -f src/migracao_cadastros_auxiliares.sql
```

Configure as variáveis de ambiente antes de iniciar:

```powershell
$env:DB_HOST = "127.0.0.1"
$env:DB_PORT = "5432"
$env:DB_NAME = "prefeitura_viagens"
$env:DB_USER = "postgres"
$env:DB_PASSWORD = "sua-senha" # obrigatório; não use credenciais no código
$env:DB_SSLMODE = "require"
```

Em produção, prefira `verify-full` e defina também `DB_SSLROOTCERT`.
O aplicativo não inicia sem `DB_PASSWORD` ou com um modo TLS que permita conexão sem criptografia.

## Compilação

```powershell
cmake -S . -B build
cmake --build build --parallel
./build/sistema_prefeitura.exe
```

São necessários Qt 6 Widgets, um compilador C++17, PostgreSQL/libpq e libpqxx.

## Versão para entrega no Windows

A pasta `Sistema-Logistica-Prefeitura` contém a versão distribuível. Ela já
inclui as DLLs do Qt, PostgreSQL/libpqxx e os plugins necessários; portanto,
não exige MSYS2 nem a inclusão de pastas no `PATH` do Windows.

Defina as variáveis `DB_*` no mesmo PowerShell que iniciará o sistema e execute:

```powershell
.\Sistema-Logistica-Prefeitura\sistema_prefeitura.exe
```

As credenciais não são distribuídas junto com o aplicativo e não devem ser
salvas no código-fonte.

## Testes automatizados

Após compilar, execute os testes de regras de CPF e datas:

```powershell
ctest --test-dir build --output-on-failure
```
