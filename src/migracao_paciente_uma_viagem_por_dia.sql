BEGIN;

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

DROP TRIGGER IF EXISTS trg_viagem_pacientes_uma_viagem_por_dia ON Viagem_Pacientes;
CREATE TRIGGER trg_viagem_pacientes_uma_viagem_por_dia
BEFORE INSERT OR UPDATE OF id_viagem, id_paciente ON Viagem_Pacientes
FOR EACH ROW EXECUTE FUNCTION impedir_paciente_em_duas_viagens_no_mesmo_dia();

DROP TRIGGER IF EXISTS trg_viagens_uma_viagem_por_dia ON Viagens;
CREATE TRIGGER trg_viagens_uma_viagem_por_dia
BEFORE UPDATE OF data_viagem ON Viagens
FOR EACH ROW EXECUTE FUNCTION impedir_paciente_em_duas_viagens_no_mesmo_dia();

COMMIT;
