-- Posturas do Bloodmage: troca mutua e gates de forma.
-- Um spell pode ter mais de um ScriptName (UNIQUE spell_id+ScriptName).
DELETE FROM `spell_script_names` WHERE `ScriptName` IN ('spell_avalon_form_stance', 'spell_avalon_form_gate');

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(90003, 'spell_avalon_form_stance'),   -- Fleshbound Form
(90004, 'spell_avalon_form_stance'),   -- Wolfbound Form
(90019, 'spell_avalon_form_stance'),   -- Unbound Curse
(90013, 'spell_avalon_form_gate'),     -- Crimson Eclipse  (exige 90003)
(90014, 'spell_avalon_form_gate'),     -- Arterial Burst   (exige 90003)
(90015, 'spell_avalon_form_gate'),     -- Talonsweep       (exige 90004)
(90016, 'spell_avalon_form_gate'),     -- Maul Bite        (exige 90004)
(90017, 'spell_avalon_form_gate'),     -- Savage Leap      (exige 90004)
(90018, 'spell_avalon_form_gate'),     -- Dread Howl       (exige 90004)
(90019, 'spell_avalon_form_gate');     -- Unbound Curse    (exige 90004)
