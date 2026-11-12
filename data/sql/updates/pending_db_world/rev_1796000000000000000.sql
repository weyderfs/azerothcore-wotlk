-- Liga o SpellScript do Veinburst (90001) a spell.
-- Sem esta linha o core loga "Script named 'spell_avalon_veinburst' is not assigned
-- in the database" e o efeito de consumir Blood Shards nunca roda.
DELETE FROM `spell_script_names` WHERE `spell_id` = 90001;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (90001, 'spell_avalon_veinburst');
