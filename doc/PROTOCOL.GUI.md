# Zappy — GUI Protocol (v3.3)

> Source de vérité : `G-YEP-400_zappy_GUI_protocol.pdf`. En cas de divergence,
> le PDF fait foi. Ce document est la référence des schémas déclarés dans
> `common/Protocol/GuiProtocol.hpp`.

Le GUI se connecte en TCP, envoie `GRAPHIC\n` lors de la poignée de main, puis
échange des lignes terminées par `\n`. Le serveur **pousse** la majorité des
messages (events) ; le GUI n'envoie qu'un petit nombre de **requêtes**.

## Légende des symboles

| Symbole | Signification |
|:-------:|---------------|
| `X` | largeur ou position horizontale |
| `Y` | hauteur ou position verticale |
| `n` | numéro de joueur |
| `O` | orientation : 1(N), 2(E), 3(S), 4(W) |
| `L` | niveau du joueur ou de l'incantation |
| `e` | numéro d'œuf |
| `T` | unité de temps |
| `N` | nom d'une équipe |
| `R` | résultat d'incantation |
| `M` | message |
| `i` | numéro de ressource |
| `q0` | ressource 0 (food) |
| `q1` | ressource 1 (linemate) |
| `q2` | ressource 2 (deraumere) |
| `q3` | ressource 3 (sibur) |
| `q4` | ressource 4 (mendiane) |
| `q5` | ressource 5 (phiras) |
| `q6` | ressource 6 (thystame) |

## Messages

| Serveur → GUI | GUI → Serveur | Détail |
|---------------|---------------|--------|
| `msz X Y\n` | `msz\n` | taille de la carte |
| `bct X Y q0 q1 q2 q3 q4 q5 q6\n` | `bct X Y\n` | contenu d'une tuile |
| `bct X Y q0 q1 q2 q3 q4 q5 q6\n` × nbr_tiles | `mct\n` | contenu de toute la carte |
| `tna N\n` × nbr_teams | `tna\n` | noms de toutes les équipes |
| `pnw #n X Y O L N\n` | — | connexion d'un nouveau joueur |
| `ppo #n X Y O\n` | `ppo #n\n` | position d'un joueur |
| `plv #n L\n` | `plv #n\n` | niveau d'un joueur |
| `pin #n X Y q0 q1 q2 q3 q4 q5 q6\n` | `pin #n\n` | inventaire d'un joueur |
| `pex #n\n` | — | expulsion |
| `pbc #n M\n` | — | broadcast |
| `pic X Y L #n #n ...\n` | — | début d'incantation (premier joueur) |
| `pie X Y R\n` | — | fin d'incantation |
| `pfk #n\n` | — | ponte d'un œuf par le joueur |
| `pdr #n i\n` | — | dépôt de ressource |
| `pgt #n i\n` | — | ramassage de ressource |
| `pdi #n\n` | — | mort d'un joueur |
| `enw #e #n X Y\n` | — | un œuf a été pondu par un joueur |
| `ebo #e\n` | — | connexion d'un joueur pour un œuf |
| `edi #e\n` | — | mort d'un œuf |
| `sgt T\n` | `sgt\n` | requête de l'unité de temps |
| `sst T\n` | `sst T\n` | modification de l'unité de temps |
| `seg N\n` | — | fin de partie |
| `smg M\n` | — | message du serveur |
| `suc\n` | — | commande inconnue |
| `sbp\n` | — | paramètre de commande invalide |
