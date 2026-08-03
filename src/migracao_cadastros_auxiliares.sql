
BEGIN;
LOCK TABLE Acompanhantes, Motoristas, Carros IN ACCESS EXCLUSIVE MODE;

DO $$
BEGIN
    IF EXISTS (SELECT 1 FROM Acompanhantes WHERE NOT cpf_valido(regexp_replace(cpf, '[^0-9]', '', 'g')))
       OR EXISTS (SELECT 1 FROM Motoristas WHERE NOT cpf_valido(regexp_replace(cpf, '[^0-9]', '', 'g'))) THEN
        RAISE EXCEPTION 'Há CPFs inválidos em Acompanhantes ou Motoristas.';
    END IF;
    IF EXISTS (SELECT 1 FROM Acompanhantes WHERE btrim(nome) = '' OR (telefone IS NOT NULL AND regexp_replace(telefone, '[^0-9]', '', 'g') !~ '^[0-9]{10,11}$'))
       OR EXISTS (SELECT 1 FROM Motoristas WHERE btrim(nome) = '')
       OR EXISTS (SELECT 1 FROM Carros WHERE btrim(modelo) = '' OR upper(regexp_replace(placa, '[^A-Za-z0-9]', '', 'g')) !~ '^[A-Z]{3}[0-9][A-Z0-9][0-9]{2}$') THEN
        RAISE EXCEPTION 'Há nomes, telefones ou placas inválidos nos cadastros auxiliares.';
    END IF;
END;
$$;

UPDATE Acompanhantes SET cpf = regexp_replace(cpf, '[^0-9]', '', 'g'), nome = btrim(nome), telefone = NULLIF(regexp_replace(telefone, '[^0-9]', '', 'g'), '');
UPDATE Motoristas SET cpf = regexp_replace(cpf, '[^0-9]', '', 'g'), nome = btrim(nome);
UPDATE Carros SET placa = upper(regexp_replace(placa, '[^A-Za-z0-9]', '', 'g')), modelo = btrim(modelo);

ALTER TABLE Acompanhantes ALTER COLUMN cpf TYPE CHAR(11) USING cpf::CHAR(11), ALTER COLUMN telefone TYPE VARCHAR(11) USING telefone::VARCHAR(11);
ALTER TABLE Motoristas ALTER COLUMN cpf TYPE CHAR(11) USING cpf::CHAR(11);
ALTER TABLE Carros ALTER COLUMN placa TYPE VARCHAR(7) USING placa::VARCHAR(7);

ALTER TABLE Acompanhantes DROP CONSTRAINT IF EXISTS ck_acompanhantes_cpf, DROP CONSTRAINT IF EXISTS ck_acompanhantes_nome, DROP CONSTRAINT IF EXISTS ck_acompanhantes_telefone,
    ADD CONSTRAINT ck_acompanhantes_cpf CHECK (cpf_valido(cpf)), ADD CONSTRAINT ck_acompanhantes_nome CHECK (btrim(nome) <> ''), ADD CONSTRAINT ck_acompanhantes_telefone CHECK (telefone IS NULL OR telefone ~ '^[0-9]{10,11}$');
ALTER TABLE Motoristas DROP CONSTRAINT IF EXISTS ck_motoristas_cpf, DROP CONSTRAINT IF EXISTS ck_motoristas_nome,
    ADD CONSTRAINT ck_motoristas_cpf CHECK (cpf_valido(cpf)), ADD CONSTRAINT ck_motoristas_nome CHECK (btrim(nome) <> '');
ALTER TABLE Carros DROP CONSTRAINT IF EXISTS ck_carros_placa, DROP CONSTRAINT IF EXISTS ck_carros_modelo,
    ADD CONSTRAINT ck_carros_placa CHECK (placa ~ '^[A-Z]{3}[0-9][A-Z0-9][0-9]{2}$'), ADD CONSTRAINT ck_carros_modelo CHECK (btrim(modelo) <> '');
COMMIT;
