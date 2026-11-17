-- rev_1805000000000000000_bloodmage_levels: Apply bloodmage_class_spells level progression table
-- (Plano 6, Task 6.7). Mapeia cada spell da classe 13 ao nível em que é concedida.
-- Tabela original: wow-core/modules/mod-custom-classes/data/bloodmage_levels.sql
CREATE TABLE IF NOT EXISTS `avalon_class_spells` (
  `class` TINYINT UNSIGNED NOT NULL,
  `spell` INT UNSIGNED NOT NULL,
  `level` TINYINT UNSIGNED NOT NULL,
  PRIMARY KEY (`class`, `spell`)
);

INSERT INTO `avalon_class_spells` (`class`, `spell`, `level`) VALUES
  (13, 90000, 2),
  (13, 90001, 10),
  (13, 90002, 6),
  (13, 90003, 12),
  (13, 90004, 20),
  (13, 90005, 1),
  (13, 90006, 4),
  (13, 90007, 8),
  (13, 90008, 14),
  (13, 90009, 18),
  (13, 90011, 22),
  (13, 90012, 26),
  (13, 90013, 30),
  (13, 90014, 36),
  (13, 90015, 40),
  (13, 90016, 42),
  (13, 90017, 46),
  (13, 90018, 50),
  (13, 90019, 54),
  (13, 90020, 58),
  (13, 90021, 62),
  (13, 90022, 66),
  (13, 90023, 70);