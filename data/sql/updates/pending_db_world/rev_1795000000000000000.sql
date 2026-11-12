-- High Elf (race 12): cria as 21 classes Avalon (12..32) no alpha.
-- Stats/acoes/posicao inicial clonados de uma classe doadora (ver DONOR no gerador).
-- Idempotente: DELETE antes de cada INSERT.

-- classe 12 <- doadora 1
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=12;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 12, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=1;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=12;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 12, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=1;
DELETE FROM `player_class_stats` WHERE `class`=12;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 12, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=1;

-- classe 13 <- doadora 8
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=13;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 13, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=8;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=13;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 13, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=8;
DELETE FROM `player_class_stats` WHERE `class`=13;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 13, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=8;

-- classe 14 <- doadora 8
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=14;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 14, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=8;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=14;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 14, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=8;
DELETE FROM `player_class_stats` WHERE `class`=14;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 14, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=8;

-- classe 15 <- doadora 9
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=15;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 15, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=9;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=15;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 15, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=9;
DELETE FROM `player_class_stats` WHERE `class`=15;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 15, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=9;

-- classe 16 <- doadora 9
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=16;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 16, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=9;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=16;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 16, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=9;
DELETE FROM `player_class_stats` WHERE `class`=16;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 16, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=9;

-- classe 17 <- doadora 1
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=17;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 17, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=1;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=17;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 17, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=1;
DELETE FROM `player_class_stats` WHERE `class`=17;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 17, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=1;

-- classe 18 <- doadora 2
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=18;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 18, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=2;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=18;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 18, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=2;
DELETE FROM `player_class_stats` WHERE `class`=18;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 18, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=2;

-- classe 19 <- doadora 9
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=19;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 19, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=9;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=19;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 19, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=9;
DELETE FROM `player_class_stats` WHERE `class`=19;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 19, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=9;

-- classe 20 <- doadora 5
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=20;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 20, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=5;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=20;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 20, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=5;
DELETE FROM `player_class_stats` WHERE `class`=20;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 20, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=5;

-- classe 21 <- doadora 8
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=21;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 21, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=8;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=21;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 21, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=8;
DELETE FROM `player_class_stats` WHERE `class`=21;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 21, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=8;

-- classe 22 <- doadora 3
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=22;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 22, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=3;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=22;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 22, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=3;
DELETE FROM `player_class_stats` WHERE `class`=22;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 22, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=3;

-- classe 23 <- doadora 4
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=23;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 23, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=4;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=23;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 23, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=4;
DELETE FROM `player_class_stats` WHERE `class`=23;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 23, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=4;

-- classe 24 <- doadora 4
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=24;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 24, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=4;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=24;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 24, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=4;
DELETE FROM `player_class_stats` WHERE `class`=24;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 24, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=4;

-- classe 25 <- doadora 5
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=25;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 25, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=5;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=25;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 25, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=5;
DELETE FROM `player_class_stats` WHERE `class`=25;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 25, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=5;

-- classe 26 <- doadora 5
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=26;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 26, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=5;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=26;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 26, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=5;
DELETE FROM `player_class_stats` WHERE `class`=26;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 26, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=5;

-- classe 27 <- doadora 5
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=27;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 27, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=5;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=27;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 27, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=5;
DELETE FROM `player_class_stats` WHERE `class`=27;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 27, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=5;

-- classe 28 <- doadora 2
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=28;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 28, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=2;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=28;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 28, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=2;
DELETE FROM `player_class_stats` WHERE `class`=28;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 28, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=2;

-- classe 29 <- doadora 4
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=29;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 29, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=4;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=29;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 29, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=4;
DELETE FROM `player_class_stats` WHERE `class`=29;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 29, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=4;

-- classe 30 <- doadora 4
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=30;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 30, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=4;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=30;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 30, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=4;
DELETE FROM `player_class_stats` WHERE `class`=30;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 30, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=4;

-- classe 31 <- doadora 5
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=31;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 31, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=5;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=31;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 31, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=5;
DELETE FROM `player_class_stats` WHERE `class`=31;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 31, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=5;

-- classe 32 <- doadora 3
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class`=32;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 32, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=1 AND `class`=3;
DELETE FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=32;
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 12, 32, `button`,`action`,`type` FROM `playercreateinfo_action` WHERE `race`=12 AND `class`=3;
DELETE FROM `player_class_stats` WHERE `class`=32;
INSERT INTO `player_class_stats` (`Class`,`Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit`)
SELECT 32, `Level`,`BaseHP`,`BaseMana`,`Strength`,`Agility`,`Stamina`,`Intellect`,`Spirit` FROM `player_class_stats` WHERE `Class`=3;

-- Classes 22 (Ranger) e 32 (Witch Hunter) usam Hunter (3) como doadora, mas a
-- race 1 nao tem Hunter. Posicao inicial vem da race 12 classe 3 (Elwynn).
DELETE FROM `playercreateinfo` WHERE `race`=12 AND `class` IN (22,32);
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 22, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=12 AND `class`=3;
INSERT INTO `playercreateinfo` (`race`,`class`,`map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation`)
SELECT 12, 32, `map`,`zone`,`position_x`,`position_y`,`position_z`,`orientation` FROM `playercreateinfo` WHERE `race`=12 AND `class`=3;
