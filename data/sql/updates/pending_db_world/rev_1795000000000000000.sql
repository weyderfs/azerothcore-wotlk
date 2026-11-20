-- High Elf (race 12): cria as 21 classes Avalon (12..32) no alpha.
--
-- Idempotente: DELETE antes de cada INSERT (re-rodar e' o jeito de corrigir).
--
-- Doadora de cada classe = `AvalonClassBase` em
-- wow-core/src/server/shared/SharedDefines.h (D-05). **Nao edite a doadora aqui**:
-- o teste tests/test_class_createinfo_sql.py compara este arquivo com o core e
-- falha se divergirem. O mapeamento ja esteve em quatro lugares diferentes e
-- discordava em seis classes (Primalist com stats de Priest e proficiencias de
-- Druid, por exemplo) - ver GAPS 5.1.7.
--
-- Tres coisas sao clonadas:
--   playercreateinfo        -> posicao inicial (literal: Elwynn, igual para todas)
--   playercreateinfo_action -> barra de acao
--   player_class_stats      -> HP/mana/atributos por nivel
--
-- Skills (proficiencias/idiomas) NAO estao aqui: vem de SkillRaceClassInfo.dbc +
-- playercreateinfo_skills, gerados por tools/build_skillraceclassinfo.py.
--
-- Barra de acao: copiada da raca 10 (Blood Elf) quando ela tem a classe - ja vem
-- com a racial 28730 (Arcane Torrent) e o mesmo layout de botoes. Warrior, Shaman
-- e Druid nao existem em Blood Elf no stock, entao vem de outra raca com a racial
-- trocada (CASE).
--
-- Posicao literal em vez de clonar de race=1: Shaman e Druid nao tem linha race=1,
-- e todas as doadoras apontam para a mesma coordenada de Elwynn de qualquer forma.

-- classe 12 Barbarian <- doadora 1 Warrior
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=12;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 12, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=12;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 12, `button`, CASE WHEN `action`=59752 THEN 28730 ELSE `action` END, `type`
FROM `playercreateinfo_action` WHERE `race`=1 AND `class`=1;
DELETE FROM `player_class_stats` WHERE `class`=12;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 12, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=1;

-- classe 13 Bloodmage <- doadora 8 Mage
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=13;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 13, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=13;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 13, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=8;
DELETE FROM `player_class_stats` WHERE `class`=13;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 13, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=8;

-- classe 14 Chronomancer <- doadora 8 Mage
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=14;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 14, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=14;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 14, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=8;
DELETE FROM `player_class_stats` WHERE `class`=14;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 14, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=8;

-- classe 15 Cultist <- doadora 9 Warlock
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=15;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 15, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=15;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 15, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=9;
DELETE FROM `player_class_stats` WHERE `class`=15;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 15, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=9;

-- classe 16 Felsworn <- doadora 9 Warlock
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=16;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 16, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=16;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 16, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=9;
DELETE FROM `player_class_stats` WHERE `class`=16;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 16, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=9;

-- classe 17 Guardian <- doadora 1 Warrior
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=17;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 17, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=17;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 17, `button`, CASE WHEN `action`=59752 THEN 28730 ELSE `action` END, `type`
FROM `playercreateinfo_action` WHERE `race`=1 AND `class`=1;
DELETE FROM `player_class_stats` WHERE `class`=17;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 17, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=1;

-- classe 18 Knight of Xoroth <- doadora 2 Paladin
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=18;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 18, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=18;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 18, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=2;
DELETE FROM `player_class_stats` WHERE `class`=18;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 18, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=2;

-- classe 19 Necromancer <- doadora 9 Warlock
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=19;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 19, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=19;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 19, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=9;
DELETE FROM `player_class_stats` WHERE `class`=19;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 19, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=9;

-- classe 20 Primalist <- doadora 11 Druid
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=20;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 20, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=20;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 20, `button`, CASE WHEN `action`=20549 THEN 28730 ELSE `action` END, `type`
FROM `playercreateinfo_action` WHERE `race`=6 AND `class`=11;
DELETE FROM `player_class_stats` WHERE `class`=20;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 20, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=11;

-- classe 21 Pyromancer <- doadora 8 Mage
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=21;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 21, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=21;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 21, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=8;
DELETE FROM `player_class_stats` WHERE `class`=21;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 21, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=8;

-- classe 22 Ranger <- doadora 3 Hunter
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=22;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 22, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=22;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 22, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=3;
DELETE FROM `player_class_stats` WHERE `class`=22;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 22, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=3;

-- classe 23 Reaper <- doadora 4 Rogue
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=23;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 23, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=23;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 23, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=4;
DELETE FROM `player_class_stats` WHERE `class`=23;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 23, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=4;

-- classe 24 Runemaster <- doadora 1 Warrior
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=24;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 24, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=24;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 24, `button`, CASE WHEN `action`=59752 THEN 28730 ELSE `action` END, `type`
FROM `playercreateinfo_action` WHERE `race`=1 AND `class`=1;
DELETE FROM `player_class_stats` WHERE `class`=24;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 24, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=1;

-- classe 25 Starcaller <- doadora 11 Druid
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=25;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 25, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=25;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 25, `button`, CASE WHEN `action`=20549 THEN 28730 ELSE `action` END, `type`
FROM `playercreateinfo_action` WHERE `race`=6 AND `class`=11;
DELETE FROM `player_class_stats` WHERE `class`=25;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 25, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=11;

-- classe 26 Stormbringer <- doadora 7 Shaman
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=26;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 26, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=26;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 26, `button`, CASE WHEN `action`=33697 THEN 28730 ELSE `action` END, `type`
FROM `playercreateinfo_action` WHERE `race`=2 AND `class`=7;
DELETE FROM `player_class_stats` WHERE `class`=26;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 26, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=7;

-- classe 27 Sun Cleric <- doadora 5 Priest
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=27;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 27, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=27;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 27, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=5;
DELETE FROM `player_class_stats` WHERE `class`=27;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 27, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=5;

-- classe 28 Templar <- doadora 2 Paladin
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=28;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 28, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=28;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 28, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=2;
DELETE FROM `player_class_stats` WHERE `class`=28;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 28, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=2;

-- classe 29 Tinker <- doadora 8 Mage
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=29;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 29, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=29;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 29, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=8;
DELETE FROM `player_class_stats` WHERE `class`=29;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 29, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=8;

-- classe 30 Venomancer <- doadora 4 Rogue
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=30;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 30, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=30;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 30, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=4;
DELETE FROM `player_class_stats` WHERE `class`=30;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 30, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=4;

-- classe 31 Witch Doctor <- doadora 7 Shaman
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=31;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 31, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=31;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 31, `button`, CASE WHEN `action`=33697 THEN 28730 ELSE `action` END, `type`
FROM `playercreateinfo_action` WHERE `race`=2 AND `class`=7;
DELETE FROM `player_class_stats` WHERE `class`=31;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 31, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=7;

-- classe 32 Witch Hunter <- doadora 3 Hunter
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=32;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
VALUES (12, 32, 0, 12, -8949.95, -132.493, 83.5312, 0);
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=32;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 32, `button`, `action`, `type`
FROM `playercreateinfo_action` WHERE `race`=10 AND `class`=3;
DELETE FROM `player_class_stats` WHERE `class`=32;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 32, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=3;
