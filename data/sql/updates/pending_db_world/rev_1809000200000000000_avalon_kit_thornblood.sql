-- Thornblood (90008): o escudo de dano passa a escalar com spell power.
DELETE FROM `spell_script_names` WHERE `spell_id` = 90008;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(90008, 'spell_avalon_thornblood');
