# Zappy — AI Protocol

> Source de vérité : `G-YEP-400_zappy.pdf`. Document de **référence
> uniquement** : le protocole IA n'est pas encore implémenté côté serveur
> (hors périmètre du portage cRPC GUI). Il est documenté ici pour cadrer
> l'intégration future.

L'IA se connecte en TCP. Contrairement au GUI (push d'events), le protocole IA
est **commande → réponse** : chaque commande reçoit `ok\n` ou `ko\n`, sauf
`Look`/`Inventory`/`Connect_nbr` qui renvoient une donnée structurée.

## Poignée de main

```
serveur → WELCOME\n
client  → <nom_équipe>\n
serveur → <nombre_de_slots_dispo>\n     (clients-nb restants pour l'équipe)
serveur → <X> <Y>\n                      (dimensions du monde)
```

## Commandes

| Commande | Temps (u.t. / f) | Réponse |
|----------|:----------------:|---------|
| `Forward\n` | 7 | `ok\n` |
| `Right\n` | 7 | `ok\n` |
| `Left\n` | 7 | `ok\n` |
| `Look\n` | 7 | `[tuile0, tuile1, ...]\n` |
| `Inventory\n` | 1 | `[linemate n, ... food n]\n` |
| `Broadcast <texte>\n` | 7 | `ok\n` |
| `Connect_nbr\n` | 0 | `<valeur>\n` (slots libres de l'équipe) |
| `Fork\n` | 42 | `ok\n` |
| `Eject\n` | 7 | `ok\n` / `ko\n` |
| `Take <objet>\n` | 7 | `ok\n` / `ko\n` |
| `Set <objet>\n` | 7 | `ok\n` / `ko\n` |
| `Incantation\n` | 300 | `Elevation underway\nCurrent level: K\n` ou `ko\n` |

## Notes

- Messages serveur non sollicités côté IA : `message <K>, <texte>\n` (broadcast
  reçu, `K` = direction du son sur le tore), `dead\n` (mort par famine).
- File de commandes : **10 max** par joueur, exécution FIFO. Au-delà, les
  commandes sont ignorées.
- `temps_action = temps_base / f` où `f` est la fréquence (`-f`, défaut 100).
