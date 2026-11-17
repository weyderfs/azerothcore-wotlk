-- Catalogo de nos de talento do Avalon (fonte de verdade do servidor).
-- Os dados sao gerados por ashes-of-avalon-client-data/tools/build_talent_dbc.py
-- (arquivo ashes-of-avalon-client-data/sql/avalon_talent_nodes.sql).
CREATE TABLE IF NOT EXISTS `avalon_talent_nodes` (
  `id`             INT UNSIGNED NOT NULL,
  `class_id`       TINYINT UNSIGNED NOT NULL,
  `spec_id`        INT UNSIGNED NOT NULL,
  `tier`           TINYINT UNSIGNED NOT NULL,
  `column_index`   TINYINT UNSIGNED NOT NULL,
  `max_rank`       TINYINT UNSIGNED NOT NULL,
  `spell`          INT UNSIGNED NOT NULL DEFAULT 0,
  `prereq`         INT UNSIGNED NOT NULL DEFAULT 0,
  `prereq_rank`    TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `required_points` INT UNSIGNED NOT NULL DEFAULT 0,
  `name`           VARCHAR(64) NOT NULL,
  `icon`           VARCHAR(96) NOT NULL DEFAULT '',
  `prereq2`        INT UNSIGNED NOT NULL DEFAULT 0,
  `prereq_rank2`   TINYINT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`),
  KEY `spec` (`spec_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Tabela ja existente em instalacoes antigas.
ALTER TABLE `avalon_talent_nodes`
  ADD COLUMN `icon` VARCHAR(96) NOT NULL DEFAULT '' AFTER `name`;
