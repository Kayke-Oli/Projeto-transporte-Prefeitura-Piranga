-- =====================================================================
-- Schema: prefeitura_viagens
-- Sistema de transporte de pacientes (TFD) - Prefeitura de Piranga
-- =====================================================================
-- Como usar:
--   createdb prefeitura_viagens
--   psql -d prefeitura_viagens -f schema.sql
-- =====================================================================

-- Tabela: Pacientes
CREATE TABLE Pacientes (
    id_paciente SERIAL PRIMARY KEY,
    cpf         VARCHAR(11)  NOT NULL UNIQUE,
    nome        VARCHAR(150) NOT NULL,
    telefone    VARCHAR(20),
    endereco    VARCHAR(255)
);

-- Tabela: Acompanhantes
CREATE TABLE Acompanhantes (
    id_acompanhante SERIAL PRIMARY KEY,
    cpf             VARCHAR(11)  NOT NULL UNIQUE,
    nome            VARCHAR(150) NOT NULL,
    telefone        VARCHAR(20)
);

-- Tabela: Motoristas
CREATE TABLE Motoristas (
    id_motorista SERIAL PRIMARY KEY,
    nome         VARCHAR(150) NOT NULL,
    cpf          VARCHAR(11)  NOT NULL UNIQUE
);

-- Tabela: Carros
CREATE TABLE Carros (
    id_carro SERIAL PRIMARY KEY,
    placa    VARCHAR(8)  NOT NULL UNIQUE, -- comporta placa antiga (AAA-9999) e Mercosul (AAA9A99)
    modelo   VARCHAR(100) NOT NULL
);

-- Tabela: Viagens
-- data_viagem usa o tipo nativo DATE do PostgreSQL. O C++ continua
-- trabalhando com o formato "DD-MM-YYYY" (Entidades.h) para o
-- resto do sistema; a conversao DD-MM-YYYY <-> YYYY-MM-DD (formato
-- ISO que o DATE exige) acontece na borda, dentro do ViagemRepository
-- (DataUtils.h). Assim ganhamos: ordenacao cronologica correta,
-- validacao automatica de datas invalidas pelo proprio banco, e
-- comparacoes corretas em BETWEEN.
CREATE TABLE Viagens (
    id_viagem      SERIAL PRIMARY KEY,
    data_viagem    DATE         NOT NULL,
    cidade_destino VARCHAR(100) NOT NULL,
    id_carro       INTEGER NOT NULL REFERENCES Carros(id_carro)       ON DELETE RESTRICT,
    id_motorista   INTEGER NOT NULL REFERENCES Motoristas(id_motorista) ON DELETE RESTRICT
);

-- Tabela associativa: Viagem_Pacientes (resolve o N:N Viagens <-> Pacientes)
-- A chave primaria composta (id_viagem, id_paciente) ja garante, a nivel
-- de banco, a regra de negocio "no maximo 1 acompanhante por paciente
-- por viagem" mencionada em PassageiroDaViagem: nao e possivel inserir
-- o mesmo paciente duas vezes na mesma viagem.
CREATE TABLE Viagem_Pacientes (
    id_viagem       INTEGER NOT NULL REFERENCES Viagens(id_viagem)         ON DELETE CASCADE,
    id_paciente     INTEGER NOT NULL REFERENCES Pacientes(id_paciente)     ON DELETE RESTRICT,
    id_acompanhante INTEGER          REFERENCES Acompanhantes(id_acompanhante) ON DELETE RESTRICT,
    PRIMARY KEY (id_viagem, id_paciente)
);

-- =====================================================================
-- Indices auxiliares
-- =====================================================================
-- cpf ja tem indice implicito por causa do UNIQUE. Os indices abaixo
-- aceleram os JOINs e filtros usados nos relatorios do ViagemRepository.

-- Acelera BETWEEN em gerarRelatorioVolumePassageiros e ORDER BY em
-- gerarRelatorioHistoricoPaciente. Com DATE, esse indice agora ordena
-- e filtra cronologicamente de forma correta (antes, com VARCHAR,
-- "01-12-2026" vinha "menor" que "15-01-2026" na ordenacao textual).
CREATE INDEX idx_viagens_data_viagem ON Viagens (data_viagem);

-- Acelera os JOINs de Viagem_Pacientes -> Pacientes/Acompanhantes
CREATE INDEX idx_viagem_pacientes_id_paciente     ON Viagem_Pacientes (id_paciente);
CREATE INDEX idx_viagem_pacientes_id_acompanhante ON Viagem_Pacientes (id_acompanhante);

-- Acelera os JOINs de Viagens -> Carros/Motoristas
CREATE INDEX idx_viagens_id_carro     ON Viagens (id_carro);
CREATE INDEX idx_viagens_id_motorista ON Viagens (id_motorista);