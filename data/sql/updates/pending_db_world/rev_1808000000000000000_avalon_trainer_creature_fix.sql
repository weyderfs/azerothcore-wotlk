-- Fix do creature_template do Blood Mage Trainer (90113).
--
-- `faction` 0 e' invalido e o worldserver loga:
--   Creature (Entry: 90113) has non-existing faction template (0).
--   Creature (Entry: 90113) has invalid unit_class (0) in creature_template. Set to 1 (CLASS_WARRIOR).
--   Creature (template id: 90113) has invalid faction (faction template id) #0
--
-- Valores copiados de class trainers stock da Alliance (ex.: 198 Khelden Bremen,
-- Mage Trainer: faction 12, unit_class 1).

UPDATE `creature_template`
SET `faction` = 12, `unit_class` = 1
WHERE `entry` = 90113;
