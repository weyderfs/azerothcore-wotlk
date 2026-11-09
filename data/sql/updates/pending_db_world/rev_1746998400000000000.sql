-- Custom Class (ID 12) - initial database injection
-- Phase 3 of the project plan

-- NOTE: Values below are placeholders. Adjust race, map, zone, position, stats,
-- starting items and spells to match the custom class design (Phase 2 DBCs).
--
-- Tables match this core: player_class_stats (class+level) and
-- playercreateinfo_spell_custom (race/class masks).

-- playercreateinfo: starting location for the custom class
DELETE FROM `playercreateinfo` WHERE `race` = 1 AND `class` = 12;
INSERT INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
VALUES (1, 12, 0, 1, -8840.94, 547.42, 94.57, 1.23);

-- player_class_stats: per-level stats, HP and mana
DELETE FROM `player_class_stats` WHERE `Class` = 12 AND `Level` BETWEEN 1 AND 80;
INSERT INTO `player_class_stats` (`Class`, `Level`, `BaseHP`, `BaseMana`, `Strength`, `Agility`, `Stamina`, `Intellect`, `Spirit`)
SELECT 12, `Level`, `BaseHP`, `BaseMana`, `Strength`, `Agility`, `Stamina`, `Intellect`, `Spirit`
FROM `player_class_stats` WHERE `Class` = 1 AND `Level` BETWEEN 1 AND 80;

-- playercreateinfo_item: starting equipment
DELETE FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 12;
INSERT INTO `playercreateinfo_item` (`race`, `class`, `itemid`, `amount`)
VALUES (1, 12, 1, 1),
       (1, 12, 2, 1);

-- playercreateinfo_spell_custom: starting spells (class mask 12 = 1 << 11 = 2048)
DELETE FROM `playercreateinfo_spell_custom` WHERE `racemask` = 0 AND `classmask` = 2048;
INSERT INTO `playercreateinfo_spell_custom` (`racemask`, `classmask`, `Spell`, `Note`)
VALUES (0, 2048, 1, 'Custom class placeholder'),
       (0, 2048, 2, 'Custom class placeholder');
