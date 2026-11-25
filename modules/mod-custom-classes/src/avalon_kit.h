#ifndef AVALON_KIT_H
#define AVALON_KIT_H

#include <string>

#include "Optional.h"

class ChatHandler;
class Player;

/// Jogador online por nome; com nome vazio, o proprio handler (console nao tem).
/// Vive aqui porque o `.avalon` inteiro precisa da mesma resolucao.
Player* AvalonResolvePlayer(ChatHandler* handler, std::string const& name);

/// `.avalon probe <player> [entry]` — estado observavel em linhas chave=valor.
/// `entry` e' `Optional<uint32>`: o parser do ChatCommands recusa token nao
/// numerico antes de chegar aqui (antes era `std::stoul`, que lancava e
/// derrubava o worldserver).
bool HandleKitProbe(ChatHandler* handler, std::string const& playerName,
    Optional<uint32> targetEntry);

/// `.avalon cast <player> <spellId> [entry]` — casta para si ou para a criatura
/// mais proxima com `entry` em 30 yd. Os `.cast` do core sao Console::No.
/// Assincrono: devolve o resultado do CheckCast, nao o fim da conjuracao; o
/// driver le `cast=1` no probe e espera o efeito.
bool HandleKitCast(ChatHandler* handler, std::string const& playerName,
    uint32 spellId, Optional<uint32> creatureEntry);

/// `.avalon set <player> <hpPct> <ragePct>` — prepara o cenario de teste.
bool HandleKitSet(ChatHandler* handler, std::string const& playerName,
    uint32 hpPercent, uint32 ragePercent);

#endif
