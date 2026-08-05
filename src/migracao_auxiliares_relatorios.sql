BEGIN;

CREATE TABLE IF NOT EXISTS Auxiliares_Viagem (
    id_auxiliar SERIAL PRIMARY KEY,
    cpf         CHAR(11)     NOT NULL UNIQUE CHECK (cpf_valido(cpf)),
    nome        VARCHAR(150) NOT NULL CHECK (btrim(nome) <> ''),
    telefone    VARCHAR(11)  CHECK (telefone IS NULL OR telefone ~ '^[0-9]{10,11}$')
);

ALTER TABLE Viagens
    ADD COLUMN IF NOT EXISTS id_auxiliar INTEGER REFERENCES Auxiliares_Viagem(id_auxiliar) ON DELETE RESTRICT;

ALTER TABLE Viagem_Pacientes
    ADD COLUMN IF NOT EXISTS id_acompanhante_paciente INTEGER REFERENCES Pacientes(id_paciente) ON DELETE RESTRICT;

ALTER TABLE Viagem_Pacientes
    DROP CONSTRAINT IF EXISTS ck_viagem_pacientes_um_acompanhante,
    ADD CONSTRAINT ck_viagem_pacientes_um_acompanhante
        CHECK (num_nonnulls(id_acompanhante, id_acompanhante_paciente) <= 1);

CREATE INDEX IF NOT EXISTS idx_viagens_id_auxiliar ON Viagens (id_auxiliar);
CREATE INDEX IF NOT EXISTS idx_viagem_pacientes_acompanhante_paciente
    ON Viagem_Pacientes (id_acompanhante_paciente);

COMMIT;
