-- Trainer do Bloodmage passa a vender apenas habilidades neutras.
-- Assinaturas de spec sao concedidas pelo servidor conforme a spec escolhida.
DELETE FROM `trainer_spell` WHERE `TrainerId` = 1305 AND `SpellId` NOT IN (
    90201, -- Veinburst
    90203, -- Fleshbound Form
    90211  -- Bloodbound Oath
);
