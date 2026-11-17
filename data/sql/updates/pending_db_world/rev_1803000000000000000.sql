-- NPC de talentos do Avalon: gossip em vez de comando de chat (o chat da race
-- 12 continua bloqueado no cliente; GAPS C-24).
DELETE FROM `creature` WHERE `id` = 900000;
DELETE FROM `creature_template` WHERE `entry` = 900000;
DELETE FROM `npc_text` WHERE `ID` = 900000;
DELETE FROM `gossip_menu` WHERE `MenuID` = 900000;

INSERT INTO `npc_text` (`ID`, `text0_0`, `text0_1`)
VALUES (900000, 'The curse answers only to those who feed it. Choose your path or spend what you have earned.', 'The curse answers only to those who feed it.');

INSERT INTO `gossip_menu` (`MenuID`, `TextID`) VALUES (900000, 900000);

CREATE TEMPORARY TABLE `avalon_trainer` AS
    SELECT * FROM `creature_template` WHERE `entry` = 198;
UPDATE `avalon_trainer`
   SET `entry` = 900000,
       `name` = 'Avalon Trainer',
       `subname` = 'Rites of the Curse',
       `npcflag` = 17,   -- 1 gossip + 16 trainer
       `faction` = 35,
       `gossip_menu_id` = 900000,
       `ScriptName` = 'npc_avalon_talents',
       `minlevel` = 1,
       `maxlevel` = 1;
INSERT INTO `creature_template` SELECT * FROM `avalon_trainer`;
DROP TEMPORARY TABLE `avalon_trainer`;

-- O clone de creature_template NAO traz o modelo: ele vive em
-- creature_template_model. Sem isso o core descarta a criatura no boot.
DELETE FROM `creature_template_model` WHERE `CreatureID` = 900000;
INSERT INTO `creature_template_model`
    (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`)
SELECT 900000, 0, 21955, 1.0, 1, 12340;   -- Argent Tome: livro voador

-- Duas copias: ponto de partida da race 12 (Northshire) e onde o hearthstone
-- deixa o personagem (Elwynn).
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `MovementType`)
VALUES
  (900000, 0, 12, 0, -8942.0, -133.0, 83.6, 0.0, 300, 0, 0),
  (900000, 0, 12, 0, -9282.0, -210.0, 70.8, 0.0, 300, 0, 0);

-- Trainer nativo: o jogador compra as spells pela janela de treino normal.
DELETE FROM `creature_default_trainer` WHERE `CreatureId` = 900000;
DELETE FROM `trainer_spell` WHERE `TrainerId` = 1305;
DELETE FROM `trainer` WHERE `Id` = 1305;

INSERT INTO `trainer` (`Id`, `Type`, `Requirement`, `Greeting`, `VerifiedBuild`)
VALUES (1305, 0, 13, 'The curse answers to those who feed it.', 12340);

INSERT INTO `creature_default_trainer` (`CreatureId`, `TrainerId`) VALUES (900000, 1305);

INSERT INTO `trainer_spell` (`TrainerId`, `SpellId`, `MoneyCost`, `ReqSkillLine`, `ReqSkillRank`, `ReqAbility1`, `ReqAbility2`, `ReqAbility3`, `ReqLevel`, `VerifiedBuild`) VALUES
(1305, 90205, 0, 0, 0, 0, 0, 0, 1, 12340),  -- Vein Lance
(1305, 90206, 0, 0, 0, 0, 0, 0, 4, 12340),  -- Wounddrinker
(1305, 90202, 0, 0, 0, 0, 0, 0, 6, 12340),  -- Hemostasis
(1305, 90207, 0, 0, 0, 0, 0, 0, 8, 12340),  -- Suture
(1305, 90201, 0, 0, 0, 0, 0, 0, 10, 12340),  -- Veinburst
(1305, 90203, 0, 0, 0, 0, 0, 0, 12, 12340),  -- Fleshbound Form
(1305, 90208, 0, 0, 0, 0, 0, 0, 14, 12340),  -- Thornblood
(1305, 90209, 0, 0, 0, 0, 0, 0, 18, 12340),  -- Coagulum
(1305, 90204, 0, 0, 0, 0, 0, 0, 20, 12340),  -- Wolfbound Form
(1305, 90211, 0, 0, 0, 0, 0, 0, 22, 12340),  -- Bloodbound Oath
(1305, 90212, 0, 0, 0, 0, 0, 0, 26, 12340),  -- Clotward
(1305, 90213, 0, 0, 0, 0, 0, 0, 30, 12340),  -- Crimson Eclipse
(1305, 90214, 0, 0, 0, 0, 0, 0, 36, 12340),  -- Arterial Burst
(1305, 90215, 0, 0, 0, 0, 0, 0, 40, 12340),  -- Talonsweep
(1305, 90216, 0, 0, 0, 0, 0, 0, 42, 12340),  -- Maul Bite
(1305, 90217, 0, 0, 0, 0, 0, 0, 46, 12340),  -- Savage Leap
(1305, 90218, 0, 0, 0, 0, 0, 0, 50, 12340),  -- Dread Howl
(1305, 90219, 0, 0, 0, 0, 0, 0, 54, 12340),  -- Unbound Curse
(1305, 90220, 0, 0, 0, 0, 0, 0, 58, 12340),  -- Skin Graft
(1305, 90221, 0, 0, 0, 0, 0, 0, 62, 12340),  -- Crimson Communion
(1305, 90223, 0, 0, 0, 0, 0, 0, 70, 12340);  -- Sanguine Rite
