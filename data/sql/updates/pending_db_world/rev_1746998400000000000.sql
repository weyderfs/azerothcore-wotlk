-- Custom Class (ID 12) - initial database injection
-- Phase 3 of the project plan

-- NOTE: Values below are placeholders. Adjust race, map, zone, position, stats,
-- starting items and spells to match the custom class design (Phase 2 DBCs).

-- playercreateinfo: starting location for the custom class
DELETE FROM `playercreateinfo` WHERE `race` = 1 AND `class` = 12;
INSERT INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
VALUES (1, 12, 0, 1, -8840.94, 547.42, 94.57, 1.23);

-- player_levelstats: base attribute gain per level (all classes share base values)
DELETE FROM `player_levelstats` WHERE `class` = 12 AND `level` BETWEEN 1 AND 80;
INSERT INTO `player_levelstats` (`class`, `level`, `strength`, `agility`, `stamina`, `intellect`, `spirit`)
SELECT 12, `level`, `strength`, `agility`, `stamina`, `intellect`, `spirit`
FROM `player_levelstats` WHERE `class` = 1 AND `level` BETWEEN 1 AND 80;

-- player_classlevelstats: class-specific modifiers per level
DELETE FROM `player_classlevelstats` WHERE `class` = 12 AND `level` BETWEEN 1 AND 80;
INSERT INTO `player_classlevelstats` (`class`, `level`, `basehealth`, `basemana`)
SELECT 12, `level`, `basehealth`, `basemana`
FROM `player_classlevelstats` WHERE `class` = 1 AND `level` BETWEEN 1 AND 80;

-- playercreateinfo_item: starting equipment
DELETE FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 12;
INSERT INTO `playercreateinfo_item` (`race`, `class`, `itemid`, `amount`)
VALUES (1, 12, 1, 1),
       (1, 12, 2, 1);

-- playercreateinfo_spell: starting spells
DELETE FROM `playercreateinfo_spell` WHERE `race` = 1 AND `class` = 12;
INSERT INTO `playercreateinfo_spell` (`race`, `class`, `spell`)
VALUES (1, 12, 1),
       (1, 12, 2);
