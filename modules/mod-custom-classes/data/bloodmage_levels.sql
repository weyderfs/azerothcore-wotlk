-- rev_1805000000000000000: Bloodmage progressão por nível (Plano 6, Task 6.7)
--
-- Tabela `avalon_class_spells` mapeia cada spell do kit Bloodmage (class=13)
-- ao nível em que é concedida. O módulo `ModAvalonBloodmageProgressao`
-- escuta `OnPlayerLevelChanged` e concede o que estiver na tabela até o nível
-- atual; no login reconcilia o que faltar (`HasSpell` antes de `learnSpell`).
--
-- Sem ranks: dano/cura crescem por coeficiente (SP, AP, SPI), não por Rank.
-- 90010 (Overflow) é aplicado pelo cap de 90000, não é aprendido pelo jogador.
--
-- Fonte: docs/research/bloodmage-full-kit.md §5

CREATE TABLE IF NOT EXISTS `avalon_class_spells` (
  `class` TINYINT UNSIGNED NOT NULL,
  `spell` INT UNSIGNED NOT NULL,
  `level` TINYINT UNSIGNED NOT NULL,
  PRIMARY KEY (`class`, `spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO `avalon_class_spells` (`class`, `spell`, `level`) VALUES
  -- Contador
  (13, 90000, 2),   -- Crimson Debt
  -- Ativas
  (13, 90001, 10),  -- Veinburst
  (13, 90002, 6),   -- Hemostasis
  (13, 90003, 12),  -- Fleshbound Form
  (13, 90004, 20),  -- Wolfbound Form
  (13, 90005, 1),   -- Vein Lance
  (13, 90006, 4),   -- Wounddrinker
  (13, 90007, 8),   -- Suture
  (13, 90008, 14),  -- Thornblood
  (13, 90009, 18),  -- Coagulum
  (13, 90011, 22),  -- Bloodbound Oath
  (13, 90012, 26),  -- Clotward
  (13, 90013, 30),  -- Crimson Eclipse
  (13, 90014, 36),  -- Arterial Burst
  (13, 90015, 40),  -- Talonsweep
  (13, 90016, 42),  -- Maul Bite
  (13, 90017, 46),  -- Savage Leap
  (13, 90018, 50),  -- Dread Howl
  (13, 90019, 54),  -- Unbound Curse
  (13, 90020, 58),  -- Skin Graft
  (13, 90021, 62),  -- Crimson Communion
  (13, 90022, 66),  -- Vital Reserve
  (13, 90023, 70);  -- Sanguine Rite

-- 90010 (Overflow) NÃO é aprendido: é debuff aplicado pelo cap de 90000.