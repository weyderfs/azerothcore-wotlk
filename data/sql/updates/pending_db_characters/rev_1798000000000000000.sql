-- Idiomas para personagens de High Elf (race 12) criados antes de
-- `playercreateinfo_skills` cobrir a raça.
--
-- Sem Common (98) o cliente exibe "Unknown Language" e a mensagem de chat nem
-- chega ao servidor — o que bloqueia inclusive comandos GM digitados no jogo.
-- Thalassian (137) entra junto por ser o idioma racial (a raça é reskin de
-- Blood Elf).
INSERT IGNORE INTO `character_skills` (`guid`, `skill`, `value`, `max`)
SELECT `c`.`guid`, `s`.`skill`, 300, 300
FROM `characters` `c`
JOIN (SELECT 98 AS `skill` UNION ALL SELECT 137) `s`
WHERE `c`.`race` = 12;
