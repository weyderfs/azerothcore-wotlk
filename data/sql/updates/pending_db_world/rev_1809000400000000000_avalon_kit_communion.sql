-- Crimson Communion (90021): gera 10 Rage no cast.
DELETE FROM `spell_script_names` WHERE `spell_id` = 90021;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(90021, 'spell_avalon_communion');
