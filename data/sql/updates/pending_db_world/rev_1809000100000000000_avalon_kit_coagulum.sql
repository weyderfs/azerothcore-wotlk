-- O tick da area do Coagulum (90105) soma 0.15*SP no servidor.
DELETE FROM `spell_script_names` WHERE `spell_id` = 90105;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(90105, 'spell_avalon_coagulum');
