BEGIN;

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

LOCK TABLE Pacientes IN ACCESS EXCLUSIVE MODE;

DO $$
BEGIN
    IF EXISTS (
        SELECT 1
        FROM Pacientes
        WHERE NOT cpf_valido(regexp_replace(cpf, '[^0-9]', '', 'g'))
    ) THEN
        RAISE EXCEPTION 'Há CPFs inválidos em Pacientes. Corrija-os antes da migração.';
    END IF;

    IF EXISTS (
        SELECT regexp_replace(cpf, '[^0-9]', '', 'g')
        FROM Pacientes
        GROUP BY regexp_replace(cpf, '[^0-9]', '', 'g')
        HAVING COUNT(*) > 1
    ) THEN
        RAISE EXCEPTION 'Há CPFs duplicados após normalização. Corrija-os antes da migração.';
    END IF;

    IF EXISTS (
        SELECT 1
        FROM Pacientes
        WHERE btrim(nome) = ''
           OR (telefone IS NOT NULL
               AND regexp_replace(telefone, '[^0-9]', '', 'g') !~ '^[0-9]{10,11}$')
    ) THEN
        RAISE EXCEPTION 'Há nomes vazios ou telefones inválidos em Pacientes. Corrija-os antes da migração.';
    END IF;
END;
$$;

UPDATE Pacientes
SET cpf = regexp_replace(cpf, '[^0-9]', '', 'g'),
    nome = btrim(nome),
    telefone = NULLIF(regexp_replace(telefone, '[^0-9]', '', 'g'), ''),
    endereco = NULLIF(btrim(endereco), '');

ALTER TABLE Pacientes
    ALTER COLUMN cpf TYPE CHAR(11) USING cpf::CHAR(11),
    ALTER COLUMN telefone TYPE VARCHAR(11) USING telefone::VARCHAR(11);

ALTER TABLE Pacientes
    DROP CONSTRAINT IF EXISTS ck_pacientes_cpf_valido,
    DROP CONSTRAINT IF EXISTS ck_pacientes_telefone,
    DROP CONSTRAINT IF EXISTS ck_pacientes_nome,
    DROP CONSTRAINT IF EXISTS ck_pacientes_endereco,
    ADD CONSTRAINT ck_pacientes_cpf_valido CHECK (cpf_valido(cpf)),
    ADD CONSTRAINT ck_pacientes_telefone CHECK (telefone IS NULL OR telefone ~ '^[0-9]{10,11}$'),
    ADD CONSTRAINT ck_pacientes_nome CHECK (btrim(nome) <> ''),
    ADD CONSTRAINT ck_pacientes_endereco CHECK (endereco IS NULL OR btrim(endereco) <> '');

COMMIT;
