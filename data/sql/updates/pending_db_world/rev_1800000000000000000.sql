-- Scripts de dano do kit do Bloodmage (task 6.4).
DELETE FROM `spell_script_names` WHERE `ScriptName` IN
  ('spell_avalon_vein_lance', 'spell_avalon_wounddrinker',
   'spell_avalon_crimson_eclipse', 'spell_avalon_arterial_burst',
   'spell_avalon_maul_bite');

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(90005, 'spell_avalon_vein_lance'),      -- Vein Lance: custo 3% vida + 0.685 SP
(90006, 'spell_avalon_wounddrinker'),    -- Wounddrinker: gasta Crimson Debt, leech
(90013, 'spell_avalon_crimson_eclipse'), -- Crimson Eclipse: 0.5 SP + 1.0 SPI
(90014, 'spell_avalon_arterial_burst'),  -- Arterial Burst: 0.5 SP por alvo
(90016, 'spell_avalon_maul_bite');       -- Maul Bite: 0.26 AP + cura
