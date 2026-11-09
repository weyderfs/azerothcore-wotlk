-- High Elf (race 22) - world database
--
-- Raça reskin de Blood Elf (10) jogável pela Aliança (team 7).
-- Os DBCs são gerados por: python3 tools/add_reskin_race.py high_elf
-- Classes liberadas: 1,2,3,4,5,6,8,9 (mesmas do Blood Elf).
--
-- Script idempotente: pode ser executado mais de uma vez.

-- ------------------------------
-- player_race_stats
-- ------------------------------
DELETE FROM `player_race_stats` WHERE `Race` = 22;
INSERT INTO `player_race_stats` (`Race`, `Strength`, `Agility`, `Stamina`, `Intellect`, `Spirit`)
SELECT 22, `Strength`, `Agility`, `Stamina`, `Intellect`, `Spirit`
FROM `player_race_stats`
WHERE `Race` = 10;

-- ------------------------------
-- playercreateinfo
-- High Elf começa na área inicial Humana (Elwynn); Death Knight em Acherus.
-- ------------------------------
DELETE FROM `playercreateinfo` WHERE `race` = 22;
INSERT INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 22, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`
FROM `playercreateinfo`
WHERE `race` = 1 AND `class` IN (1, 2, 4, 5, 6, 8, 9);

-- Hunter (3) não existe na raça 1: usa a posição inicial de Elwynn.
DELETE FROM `playercreateinfo` WHERE `race` = 22 AND `class` = 3;
INSERT INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
VALUES (22, 3, 0, 12, -8949.95, -132.493, 83.5312, 0);

-- ------------------------------
-- playercreateinfo_action
-- Copia a barra do Blood Elf (racial Arcane Torrent = 28730).
-- ------------------------------
DELETE FROM `playercreateinfo_action` WHERE `race` = 22;
INSERT INTO `playercreateinfo_action` (`race`, `class`, `button`, `action`, `type`)
SELECT 22, `class`, `button`, `action`, `type`
FROM `playercreateinfo_action`
WHERE `race` = 10 AND `class` IN (2, 3, 4, 5, 6, 8, 9);

-- Warrior (1) não existe na raça 10: copia a barra da raça 1 e troca a racial
-- humana (59752) pela racial Blood Elf (28730).
DELETE FROM `playercreateinfo_action` WHERE `race` = 22 AND `class` = 1;
INSERT INTO `playercreateinfo_action` (`race`, `class`, `button`, `action`, `type`)
SELECT 22, `class`, `button`,
       CASE WHEN `action` = 59752 THEN 28730 ELSE `action` END,
       `type`
FROM `playercreateinfo_action`
WHERE `race` = 1 AND `class` = 1;

-- ------------------------------
-- playercreateinfo_skills
-- Idiomas e perícias raciais: High Elf herda as linhas de Blood Elf (512) e as
-- linhas válidas para todas as raças da Aliança (máscara 1101 completa).
-- ------------------------------
UPDATE `playercreateinfo_skills` SET `raceMask` = `raceMask` | 0x200000
WHERE (`raceMask` & 0x200000) = 0
  AND ((`raceMask` & 1101) = 1101 OR (`raceMask` & 512));

-- ------------------------------
-- quest_template
-- Permite High Elf nas missões abertas a todas as raças da Aliança.
-- Missões exclusivas de outra raça (ex.: 512, 1024) ficam de fora.
-- ------------------------------
UPDATE `quest_template` SET `AllowableRaces` = `AllowableRaces` | 0x200000
WHERE (`AllowableRaces` & 1101) = 1101
  AND (`AllowableRaces` & 0x200000) = 0;
