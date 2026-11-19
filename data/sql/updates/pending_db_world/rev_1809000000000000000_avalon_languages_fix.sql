-- Coerencia entre `playercreateinfo_skills` e o `SkillRaceClassInfo.dbc` corrigido.
--
-- O DBC (override) agora da' race 12 as skills que faltavam nas linhas stock:
--   45  Bows              -> raceMask 2698  (1101 Alliance|... | 2048 race 12)
--   98  Language: Common  -> raceMask 3149
--   137 Language: Thalassian -> raceMask 2560 (512 Blood Elf | 2048 race 12)
--   173 Daggers           -> linhas com 2048
--
-- Linhas com mascara so-Horde NAO tem race 12 no DBC, entao concede-las ao
-- personagem faz o servidor apagar a skill no login
-- (`Player::_LoadSkills` -> `GetSkillRaceClassInfo(skill, 12, class)` == null):
--   109 Language: Orcish    (idioma da Horda)
--   756 Blood Elf Racial    (linha stock e' so Blood Elf)
--
-- 98 e 137 ficam: Common e' o idioma compartilhado da Alliance e Thalassian e' o
-- idioma herdado dos Blood Elf (decisao do projeto).

UPDATE `playercreateinfo_skills` SET `raceMask` = `raceMask` & ~2048 WHERE `skill` IN (109, 756);
