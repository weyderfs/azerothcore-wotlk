-- Skin Graft (90020): consome Vital Reserve no cast.
DELETE FROM `spell_script_names` WHERE `spell_id` = 90020;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(90020, 'spell_avalon_skin_graft');
