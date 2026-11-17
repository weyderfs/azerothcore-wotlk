-- Adiciona coluna spec_id para avalon_class_spells, permite gate por spec
-- Remove rows antigos, re-popula base (spec_id 0) + per-spec (1301-1304)
-- Idempotente: ALTERs apenas se ainda não aplicados

ALTER TABLE `avalon_class_spells` ADD COLUMN `spec_id` int unsigned NOT NULL DEFAULT 0 AFTER `class`;

DELETE FROM `avalon_class_spells` WHERE `class` = 13;

-- base (todas as specs)
INSERT INTO `avalon_class_spells` (`class`, `spec_id`, `spell`, `level`) VALUES
(13, 0, 90000, 2),
(13, 0, 90001, 10),
(13, 0, 90003, 12),
(13, 0, 90004, 20),
(13, 0, 90011, 22);

-- Fleshweaver (1301)
INSERT INTO `avalon_class_spells` (`class`, `spec_id`, `spell`, `level`) VALUES
(13, 1301, 90002, 6),
(13, 1301, 90007, 8),
(13, 1301, 90020, 58),
(13, 1301, 90022, 66);

-- Blood (1302)
INSERT INTO `avalon_class_spells` (`class`, `spec_id`, `spell`, `level`) VALUES
(13, 1302, 90005, 1),
(13, 1302, 90009, 18),
(13, 1302, 90013, 30),
(13, 1302, 90014, 36);

-- Accursed (1303)
INSERT INTO `avalon_class_spells` (`class`, `spec_id`, `spell`, `level`) VALUES
(13, 1303, 90015, 40),
(13, 1303, 90016, 42),
(13, 1303, 90017, 46),
(13, 1303, 90018, 50),
(13, 1303, 90019, 54);

-- Eternal (1304)
INSERT INTO `avalon_class_spells` (`class`, `spec_id`, `spell`, `level`) VALUES
(13, 1304, 90006, 4),
(13, 1304, 90008, 14),
(13, 1304, 90012, 26),
(13, 1304, 90021, 62),
(13, 1304, 90023, 70);

-- PK inclui spec_id (um personagem só tem uma spec ativa)
ALTER TABLE `avalon_class_spells` DROP PRIMARY KEY, ADD PRIMARY KEY (`class`, `spec_id`, `spell`);
