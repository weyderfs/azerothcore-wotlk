-- Suture (90007) ganha script proprio: coeficiente de SP/SPI/AP na cura.
DELETE FROM `spell_script_names` WHERE `spell_id` = 90007;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(90007, 'spell_avalon_suture');
