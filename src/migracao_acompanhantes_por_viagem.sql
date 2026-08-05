BEGIN;

-- Acompanhantes novos pertencem à viagem; não precisam de cadastro prévio.
ALTER TABLE Viagem_Pacientes
    ADD COLUMN IF NOT EXISTS id_acompanhante_paciente INTEGER REFERENCES Pacientes(id_paciente) ON DELETE RESTRICT,
    ADD COLUMN IF NOT EXISTS acompanhante_nome VARCHAR(150),
    ADD COLUMN IF NOT EXISTS acompanhante_cpf CHAR(11),
    ADD COLUMN IF NOT EXISTS acompanhante_telefone VARCHAR(11);

ALTER TABLE Viagem_Pacientes
    DROP CONSTRAINT IF EXISTS ck_viagem_pacientes_um_acompanhante,
    ADD CONSTRAINT ck_viagem_pacientes_um_acompanhante
        CHECK (num_nonnulls(id_acompanhante, id_acompanhante_paciente, acompanhante_cpf) <= 1);

ALTER TABLE Viagem_Pacientes
    DROP CONSTRAINT IF EXISTS ck_viagem_pacientes_acompanhante_avulso,
    ADD CONSTRAINT ck_viagem_pacientes_acompanhante_avulso
        CHECK ((acompanhante_cpf IS NULL AND acompanhante_nome IS NULL AND acompanhante_telefone IS NULL)
            OR (acompanhante_cpf IS NOT NULL AND cpf_valido(acompanhante_cpf)
                AND btrim(acompanhante_nome) <> '' AND acompanhante_telefone ~ '^[0-9]{10,11}$'));

COMMIT;
