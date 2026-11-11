-- High Elf (race 12) - world database
--
-- A race 22 crasha o cliente 3.3.5a stock; a implementacao usa a race 12
-- (slot Fel Orc). Ver docs/research/client-limits-335.md.
--
-- Alpha (D-11): 10 classes = 1,2,3,4,5,6,8,9 (originais) + 12 Barbarian e
-- 13 Bloodmage (Avalon). Os stats das classes Avalon sao clonados da classe
-- base (D-05): 12 <- Warrior (1), 13 <- Mage (8).
--
-- Script idempotente: pode ser executado mais de uma vez.

-- ------------------------------
-- player_race_stats
-- ------------------------------
DELETE FROM `player_race_stats` WHERE `Race` = 12;
INSERT INTO `player_race_stats` (`Race`, `Strength`, `Agility`, `Stamina`, `Intellect`, `Spirit`)
SELECT 12, `Strength`, `Agility`, `Stamina`, `Intellect`, `Spirit`
FROM `player_race_stats`
WHERE `Race` = 10;

-- ------------------------------
-- player_class_stats
-- HP/mana/stats por level das 2 classes Avalon do alpha.
-- ------------------------------
DELETE FROM `player_class_stats` WHERE `Class` IN (12, 13);
INSERT INTO `player_class_stats` (`Class`, `Level`, `BaseHP`, `BaseMana`, `Strength`, `Agility`, `Stamina`, `Intellect`, `Spirit`)
SELECT 12, `Level`, `BaseHP`, `BaseMana`, `Strength`, `Agility`, `Stamina`, `Intellect`, `Spirit`
FROM `player_class_stats`
WHERE `Class` = 1;
INSERT INTO `player_class_stats` (`Class`, `Level`, `BaseHP`, `BaseMana`, `Strength`, `Agility`, `Stamina`, `Intellect`, `Spirit`)
SELECT 13, `Level`, `BaseHP`, `BaseMana`, `Strength`, `Agility`, `Stamina`, `Intellect`, `Spirit`
FROM `player_class_stats`
WHERE `Class` = 8;

-- ------------------------------
-- playercreateinfo
-- High Elf comeca na area inicial Humana (Elwynn); Death Knight em Acherus.
-- ------------------------------
DELETE FROM `playercreateinfo` WHERE `race` = 12;
INSERT INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 12, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`
FROM `playercreateinfo`
WHERE `race` = 1 AND `class` IN (1, 2, 4, 5, 6, 8, 9);

-- Hunter (3) nao existe na raca 1: usa a posicao inicial de Elwynn.
DELETE FROM `playercreateinfo` WHERE `race` = 12 AND `class` = 3;
INSERT INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
VALUES (12, 3, 0, 12, -8949.95, -132.493, 83.5312, 0);

-- Barbarian (12) e Bloodmage (13): herdam a posicao inicial de Warrior/Mage.
DELETE FROM `playercreateinfo` WHERE `race` = 12 AND `class` IN (12, 13);
INSERT INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 12, 12, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`
FROM `playercreateinfo`
WHERE `race` = 1 AND `class` = 1;
INSERT INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 12, 13, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`
FROM `playercreateinfo`
WHERE `race` = 1 AND `class` = 8;

-- ------------------------------
-- playercreateinfo_action
-- Copia a barra do Blood Elf (racial Arcane Torrent = 28730).
-- ------------------------------
DELETE FROM `playercreateinfo_action` WHERE `race` = 12;
INSERT INTO `playercreateinfo_action` (`race`, `class`, `button`, `action`, `type`)
SELECT 12, `class`, `button`, `action`, `type`
FROM `playercreateinfo_action`
WHERE `race` = 10 AND `class` IN (2, 3, 4, 5, 6, 8, 9);

-- Warrior (1) nao existe na raca 10: copia a barra da raca 1 e troca a racial
-- humana (59752) pela racial Blood Elf (28730).
DELETE FROM `playercreateinfo_action` WHERE `race` = 12 AND `class` = 1;
INSERT INTO `playercreateinfo_action` (`race`, `class`, `button`, `action`, `type`)
SELECT 12, `class`, `button`,
       CASE WHEN `action` = 59752 THEN 28730 ELSE `action` END,
       `type`
FROM `playercreateinfo_action`
WHERE `race` = 1 AND `class` = 1;

-- Barbarian (12) e Bloodmage (13): barra de Warrior/Mage com a racial trocada.
DELETE FROM `playercreateinfo_action` WHERE `race` = 12 AND `class` IN (12, 13);
INSERT INTO `playercreateinfo_action` (`race`, `class`, `button`, `action`, `type`)
SELECT 12, 12, `button`,
       CASE WHEN `action` = 59752 THEN 28730 ELSE `action` END,
       `type`
FROM `playercreateinfo_action`
WHERE `race` = 1 AND `class` = 1;
INSERT INTO `playercreateinfo_action` (`race`, `class`, `button`, `action`, `type`)
SELECT 12, 13, `button`,
       CASE WHEN `action` = 59752 THEN 28730 ELSE `action` END,
       `type`
FROM `playercreateinfo_action`
WHERE `race` = 1 AND `class` = 8;

-- ------------------------------
-- playercreateinfo_skills
-- Idiomas e pericias raciais: High Elf herda as linhas de Blood Elf (512) e as
-- linhas validas para todas as racas da Alianca (mascara 1101 completa).
-- Bit da race 12 = 1 << 11 = 0x800.
-- ------------------------------
UPDATE `playercreateinfo_skills` SET `raceMask` = `raceMask` | 0x800
WHERE (`raceMask` & 0x800) = 0
  AND ((`raceMask` & 1101) = 1101 OR (`raceMask` & 512));

-- ------------------------------
-- quest_template
-- Permite High Elf nas missoes abertas a todas as racas da Alianca.
-- Missoes exclusivas de outra raca (ex.: 512, 1024) ficam de fora.
-- ------------------------------
UPDATE `quest_template` SET `AllowableRaces` = `AllowableRaces` | 0x800
WHERE (`AllowableRaces` & 1101) = 1101
  AND (`AllowableRaces` & 0x800) = 0;
