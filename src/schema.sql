-- =====================================================================
-- Schema: prefeitura_viagens
-- Sistema de transporte de pacientes (TFD) - Prefeitura de Piranga
-- =====================================================================
-- Como usar:
--   createdb prefeitura_viagens
--   psql -d prefeitura_viagens -f schema.sql
-- =====================================================================

-- Valida CPF já normalizado (somente 11 dígitos), incluindo os dois
-- dígitos verificadores. A UI faz a mesma validação para dar retorno rápido,
-- mas esta função garante a regra mesmo para acessos externos ao sistema.
CREATE OR REPLACE FUNCTION cpf_valido(cpf_texto TEXT)
RETURNS BOOLEAN
LANGUAGE plpgsql
IMMUTABLE
STRICT
AS $$
DECLARE
    soma INTEGER;
    resto INTEGER;
    digito_esperado INTEGER;
    posicao INTEGER;
BEGIN
    IF cpf_texto !~ '^[0-9]{11}$'
       OR cpf_texto = repeat(substr(cpf_texto, 1, 1), 11) THEN
        RETURN FALSE;
    END IF;

    soma := 0;
    FOR posicao IN 1..9 LOOP
        soma := soma + substr(cpf_texto, posicao, 1)::INTEGER * (11 - posicao);
    END LOOP;
    resto := soma % 11;
    digito_esperado := CASE WHEN resto < 2 THEN 0 ELSE 11 - resto END;
    IF substr(cpf_texto, 10, 1)::INTEGER <> digito_esperado THEN
        RETURN FALSE;
    END IF;

    soma := 0;
    FOR posicao IN 1..10 LOOP
        soma := soma + substr(cpf_texto, posicao, 1)::INTEGER * (12 - posicao);
    END LOOP;
    resto := soma % 11;
    digito_esperado := CASE WHEN resto < 2 THEN 0 ELSE 11 - resto END;

    RETURN substr(cpf_texto, 11, 1)::INTEGER = digito_esperado;
END;
$$;

-- Tabela: Pacientes
CREATE TABLE Pacientes (
    id_paciente SERIAL PRIMARY KEY,
    cpf         CHAR(11)     NOT NULL UNIQUE,
    nome        VARCHAR(150) NOT NULL CHECK (btrim(nome) <> ''),
    telefone    VARCHAR(11)  CHECK (telefone IS NULL OR telefone ~ '^[0-9]{10,11}$'),
    endereco    VARCHAR(255) CHECK (endereco IS NULL OR btrim(endereco) <> ''),
    CONSTRAINT ck_pacientes_cpf_valido CHECK (cpf_valido(cpf))
);

-- Tabela: Acompanhantes
CREATE TABLE Acompanhantes (
    id_acompanhante SERIAL PRIMARY KEY,
    cpf             CHAR(11)     NOT NULL UNIQUE CHECK (cpf_valido(cpf)),
    nome            VARCHAR(150) NOT NULL CHECK (btrim(nome) <> ''),
    telefone        VARCHAR(11)  CHECK (telefone IS NULL OR telefone ~ '^[0-9]{10,11}$')
);

-- Tabela: Auxiliares de viagem
CREATE TABLE Auxiliares_Viagem (
    id_auxiliar SERIAL PRIMARY KEY,
    cpf         CHAR(11)     NOT NULL UNIQUE CHECK (cpf_valido(cpf)),
    nome        VARCHAR(150) NOT NULL CHECK (btrim(nome) <> ''),
    telefone    VARCHAR(11)  CHECK (telefone IS NULL OR telefone ~ '^[0-9]{10,11}$')
);

-- Tabela: Motoristas
CREATE TABLE Motoristas (
    id_motorista SERIAL PRIMARY KEY,
    nome         VARCHAR(150) NOT NULL CHECK (btrim(nome) <> ''),
    cpf          CHAR(11)     NOT NULL UNIQUE CHECK (cpf_valido(cpf))
);

-- Tabela: Carros
CREATE TABLE Carros (
    id_carro SERIAL PRIMARY KEY,
    placa    VARCHAR(7)  NOT NULL UNIQUE CHECK (placa ~ '^[A-Z]{3}[0-9][A-Z0-9][0-9]{2}$'),
    modelo   VARCHAR(100) NOT NULL CHECK (btrim(modelo) <> '')
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
    cidade_destino VARCHAR(100) NOT NULL CHECK (btrim(cidade_destino) <> ''),
    id_carro       INTEGER NOT NULL REFERENCES Carros(id_carro)       ON DELETE RESTRICT,
    id_motorista   INTEGER NOT NULL REFERENCES Motoristas(id_motorista) ON DELETE RESTRICT,
    id_auxiliar    INTEGER          REFERENCES Auxiliares_Viagem(id_auxiliar) ON DELETE RESTRICT
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
    id_acompanhante_paciente INTEGER REFERENCES Pacientes(id_paciente) ON DELETE RESTRICT,
    acompanhante_nome     VARCHAR(150),
    acompanhante_cpf      CHAR(11),
    acompanhante_telefone VARCHAR(11),
    PRIMARY KEY (id_viagem, id_paciente),
    CONSTRAINT ck_viagem_pacientes_um_acompanhante
        CHECK (num_nonnulls(id_acompanhante, id_acompanhante_paciente, acompanhante_cpf) <= 1),
    CONSTRAINT ck_viagem_pacientes_acompanhante_avulso
        CHECK ((acompanhante_cpf IS NULL AND acompanhante_nome IS NULL AND acompanhante_telefone IS NULL)
            OR (acompanhante_cpf IS NOT NULL AND cpf_valido(acompanhante_cpf)
                AND btrim(acompanhante_nome) <> '' AND acompanhante_telefone ~ '^[0-9]{10,11}$'))
);

-- Um paciente pode participar de apenas uma viagem em cada data. A regra
-- tambem vale para inclusoes feitas fora da interface do sistema.
CREATE OR REPLACE FUNCTION impedir_paciente_em_duas_viagens_no_mesmo_dia()
RETURNS TRIGGER
LANGUAGE plpgsql
AS $$
DECLARE
    data_alvo DATE;
BEGIN
    IF TG_TABLE_NAME = 'viagem_pacientes' THEN
        SELECT data_viagem INTO data_alvo
        FROM Viagens
        WHERE id_viagem = NEW.id_viagem;

        IF EXISTS (
            SELECT 1
            FROM Viagem_Pacientes vp
            JOIN Viagens v ON v.id_viagem = vp.id_viagem
            WHERE vp.id_paciente = NEW.id_paciente
              AND v.data_viagem = data_alvo
              AND vp.id_viagem <> NEW.id_viagem
        ) THEN
            RAISE EXCEPTION 'O paciente ja possui uma viagem cadastrada para esta data.'
                USING ERRCODE = 'P0001';
        END IF;
    ELSE
        IF EXISTS (
            SELECT 1
            FROM Viagem_Pacientes atual
            JOIN Viagem_Pacientes outra ON outra.id_paciente = atual.id_paciente
            JOIN Viagens viagem_outra ON viagem_outra.id_viagem = outra.id_viagem
            WHERE atual.id_viagem = NEW.id_viagem
              AND outra.id_viagem <> NEW.id_viagem
              AND viagem_outra.data_viagem = NEW.data_viagem
        ) THEN
            RAISE EXCEPTION 'Um paciente desta viagem ja possui outra viagem cadastrada para esta data.'
                USING ERRCODE = 'P0001';
        END IF;
    END IF;

    RETURN NEW;
END;
$$;

CREATE TRIGGER trg_viagem_pacientes_uma_viagem_por_dia
BEFORE INSERT OR UPDATE OF id_viagem, id_paciente ON Viagem_Pacientes
FOR EACH ROW EXECUTE FUNCTION impedir_paciente_em_duas_viagens_no_mesmo_dia();

CREATE TRIGGER trg_viagens_uma_viagem_por_dia
BEFORE UPDATE OF data_viagem ON Viagens
FOR EACH ROW EXECUTE FUNCTION impedir_paciente_em_duas_viagens_no_mesmo_dia();

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
CREATE INDEX idx_viagens_id_auxiliar ON Viagens (id_auxiliar);
CREATE INDEX idx_viagem_pacientes_acompanhante_paciente ON Viagem_Pacientes (id_acompanhante_paciente);
