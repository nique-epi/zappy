# Zappy — Issues serveur (prévisualisation)

> Prévisualisation des issues à créer pour le **serveur**, rédigées avec le
> template `.github/ISSUE_TEMPLATE/issue.md` (user story).
>
> - Titres en **anglais**, corps en **français** (conforme au template).
> - **Niveau fonctionnel uniquement** : aucun détail d'implémentation (pas de
>   classes, fonctions, fichiers). Les jetons de protocole (`Forward`, `msz`,
>   `eject: K`, `ok/ko`…) sont de la **spécification**, pas de l'implémentation.
> - Les identifiants `#N` sont des **numéros de prévisualisation** ; ils seront
>   remplacés par les vrais numéros GitHub à la création (liens `Depends on` à
>   réajuster à ce moment-là).
> - Granularité **moyenne** : chaque issue est un bloc cohérent et livrable en
>   quelques jours, sans descendre au niveau « une commande = une issue ».

## Conventions de labels

- **type** : `type: feature`, `type: infra`, `type: bug`
- **priorité** : `P0 - Critical`, `P1 - High`, `P2 - Medium`, `P3 - Low`
- **taille** : `XS`, `S`, `M`, `L`, `XL`
- **milestone** : `Milestone: M1` … `Milestone: M3`

## Roadmap (3 milestones)

| Milestone | Objectif livrable | Issues |
|---|---|---|
| **M1 — Moteur & connexion** | Un monde vivant où des drones se connectent, naissent, survivent, et où le temps s'écoule sans attente active | #1 → #7 |
| **M2 — Gameplay** | Toutes les actions IA, jusqu'à l'élévation et la victoire | #8 → #13 |
| **M3 — Protocole GUI** | Le client graphique voit tout, en requête et en direct | #14 → #17 |

## Sommaire

| # | Titre | Prio | Taille | MS |
|---|---|---|---|---|
| 1 | [server] Toroidal map and tile model | P0 | M | M1 |
| 2 | [server] Resource spawning and periodic respawn | P0 | M | M1 |
| 3 | [server] Action scheduling and timing without active waiting | P0 | L | M1 |
| 4 | [server] Request buffering and per-player action blocking | P0 | M | M1 |
| 5 | [server] Client connection and AI handshake | P0 | M | M1 |
| 6 | [server] Teams, slots and egg hatching | P0 | M | M1 |
| 7 | [server] Player state, survival and death | P0 | M | M1 |
| 8 | [server] AI movement and resource actions | P1 | M | M2 |
| 9 | [server] AI perception commands | P1 | M | M2 |
| 10 | [server] Broadcast and sound propagation | P2 | M | M2 |
| 11 | [server] Reproduction and ejection | P2 | M | M2 |
| 12 | [server] Elevation ritual | P1 | L | M2 |
| 13 | [server] End of game and victory condition | P1 | S | M2 |
| 14 | [server] GUI queries: map and teams | P1 | M | M3 |
| 15 | [server] GUI queries: players and time control | P1 | M | M3 |
| 16 | [server] GUI events: players and resources | P2 | M | M3 |
| 17 | [server] GUI events: game, eggs and system | P2 | M | M3 |

---
---

# Milestone 1 — Moteur & connexion

## #1 — [server] Toroidal map and tile model

**Labels :** `type: feature` · `P0 - Critical` · `M` · `Milestone: M1`

### User Story

En tant que **moteur de jeu**, je veux **un monde torique de dimensions `width × height` dont chaque case connaît son contenu**, afin que **déplacements, vision et son reposent sur une topologie cohérente**.

### Critères d'acceptation

- [ ] Le monde a les dimensions passées en ligne de commande (`-x`, `-y`).
- [ ] Le monde est torique : franchir un bord fait réapparaître du côté opposé, sur les deux axes et aux quatre coins.
- [ ] Chaque case porte les quantités des 6 minerais (linemate, deraumere, sibur, mendiane, phiras, thystame) et de la nourriture, ainsi que les joueurs et œufs présents.
- [ ] Le contenu d'une case est consultable à tout moment.

### Notes

- Monde plat, sans relief (cf. sujet « Geography »).
- Seule la **classe** d'un objet est identifiable (deux `sibur` sont indistinguables).

### Depends on

- —

---

## #2 — [server] Resource spawning and periodic respawn

**Labels :** `type: feature` · `P0 - Critical` · `M` · `Milestone: M1`

### User Story

En tant que **moteur de jeu**, je veux **approvisionner la carte en ressources au démarrage puis périodiquement**, afin que **les drones trouvent en permanence de quoi se nourrir et progresser**.

### Critères d'acceptation

- [ ] Au démarrage, chaque ressource est distribuée en quantité `width × height × densité` (food 0.5, linemate 0.3, deraumere 0.15, sibur 0.1, mendiane 0.1, phiras 0.08, thystame 0.05).
- [ ] Au moins un exemplaire de chaque ressource est présent, réparti de façon homogène.
- [ ] Un réapprovisionnement a lieu toutes les 20 unités de temps en restaurant ces quantités cibles.

### Notes

- Exemple sujet : un monde 10×10 contient 50 food et 5 thystame.
- Le déclenchement périodique dépend de l'horloge de jeu (#3).

### Depends on

- #1, #3

---

## #3 — [server] Action scheduling and timing without active waiting

**Labels :** `type: feature` · `P0 - Critical` · `L` · `Milestone: M1`

### User Story

En tant que **serveur mono-process / mono-thread**, je veux **planifier l'exécution des actions dans le temps sans aucune attente active**, afin que **le rythme `f` soit respecté et que le CPU reste au repos quand rien ne se passe**.

### Critères d'acceptation

- [ ] Le coût d'une action vaut `durée_action / f` secondes ; les durées du sujet sont respectées (Forward/Right/Left/Look/Broadcast/Eject/Take/Set = 7/f, Inventory = 1/f, Fork = 42/f, Incantation = 300/f, Connect_nbr immédiat).
- [ ] Le multiplexage utilise `poll` ; le réveil ne se produit que sur activité réseau **ou** échéance d'un événement de jeu.
- [ ] À l'arrêt des clients, l'occupation CPU reste négligeable (aucune boucle de scrutation).

### Notes

- Critère noté par le jury (vérifiable à `strace`).
- `f` par défaut = 100, modifiable à chaud par le GUI (#15).

### Depends on

- —

---

## #4 — [server] Request buffering and per-player action blocking

**Labels :** `type: feature` · `P0 - Critical` · `M` · `Milestone: M1`

### User Story

En tant que **client IA**, je veux **enchaîner mes commandes sans attendre chaque réponse**, afin que **mon drone agisse efficacement tout en respectant les limites du serveur**.

### Critères d'acceptation

- [ ] Un client peut empiler jusqu'à 10 requêtes en attente ; au-delà, les suivantes sont ignorées.
- [ ] Les requêtes d'un client sont exécutées dans l'ordre de réception.
- [ ] Une action en cours (ou un gel d'incantation) ne bloque que son joueur ; les autres continuent d'agir.

### Notes

- Chaque commande IA se termine par un `\n`.

### Depends on

- #3

---

## #5 — [server] Client connection and AI handshake

**Labels :** `type: feature` · `P0 - Critical` · `M` · `Milestone: M1`

### User Story

En tant que **client IA ou graphique**, je veux **me connecter et être identifié par mon nom d'équipe**, afin que **je reçoive les informations d'entrée en jeu adaptées à mon rôle**.

### Critères d'acceptation

- [ ] Le serveur envoie `WELCOME` à la connexion.
- [ ] Pour une IA, après réception du nom d'équipe, il renvoie le **nombre réel de slots disponibles** puis la taille du monde `X Y`.
- [ ] Une connexion sur une équipe pleine renvoie 0 slot et n'admet pas de drone, sans déstabiliser le serveur.
- [ ] Un client se présentant comme `GRAPHIC` est routé vers le protocole GUI et ne consomme ni slot ni œuf.

### Notes

- Note d'avancement : la séquence de poignée de main et le routage GUI/IA existent déjà à l'état de squelette ; le compte de slots est à brancher sur #6.

### Depends on

- #1, #6

---

## #6 — [server] Teams, slots and egg hatching

**Labels :** `type: feature` · `P0 - Critical` · `M` · `Milestone: M1`

### User Story

En tant que **équipe**, je veux **des places disponibles matérialisées par des œufs qui éclosent à la connexion**, afin que **de nouveaux drones entrent en jeu de façon contrôlée**.

### Critères d'acceptation

- [ ] Chaque équipe dispose de `n` slots (`-c`) représentés par des œufs au démarrage.
- [ ] Le nombre de slots libres reflète à tout instant les œufs disponibles.
- [ ] Se connecter à un slot libre fait éclore un œuf choisi au hasard ; le drone apparaît à sa position, niveau 1, **direction aléatoire**.
- [ ] Pondre un œuf (Fork) ajoute un slot ; une éclosion en retire un.

### Notes

- Le nombre de slots libres est exposé à l'IA par `Connect_nbr` (#9) et à la connexion (#5).

### Depends on

- #1

---

## #7 — [server] Player state, survival and death

**Labels :** `type: feature` · `P0 - Critical` · `M` · `Milestone: M1`

### User Story

En tant que **joueur (drone)**, je veux **un état complet et une réserve de vie qui s'épuise**, afin que **je doive me nourrir pour survivre et que je meure sinon**.

### Critères d'acceptation

- [ ] Un joueur a une position, une orientation (N/E/S/O), un niveau (1→8) et un inventaire, cohérents avec le monde.
- [ ] Un joueur démarre avec 10 unités de nourriture ; 1 unité = 126 unités de temps (1260 au total) ; manger prolonge la durée de vie.
- [ ] À épuisement de la nourriture, le joueur reçoit `dead`, est retiré du monde et libère sa place.

### Notes

- Le joueur occupe toute sa case ; sa direction n'est pas distinguable par les autres.

### Depends on

- #1, #3, #6

---
---

# Milestone 2 — Gameplay

## #8 — [server] AI movement and resource actions

**Labels :** `type: feature` · `P1 - High` · `M` · `Milestone: M2`

### User Story

En tant que **drone**, je veux **me déplacer, m'orienter et manipuler les ressources de ma case**, afin que **je puisse parcourir le monde et constituer mon inventaire**.

### Critères d'acceptation

- [ ] `Forward` avance d'une case dans l'orientation courante, avec rebouclage torique → `ok`.
- [ ] `Right` / `Left` pivotent l'orientation de 90° → `ok`.
- [ ] `Take object` / `Set object` transfèrent une ressource entre la case et l'inventaire → `ok`/`ko` selon disponibilité.
- [ ] Toute commande inconnue ou mal formée renvoie `ko` sans rompre la session.

### Notes

- Toutes les réponses se terminent par `\n`.
- Note d'avancement : un repli `ko` générique existe déjà à l'état de squelette.

### Depends on

- #1, #4, #7

---

## #9 — [server] AI perception commands

**Labels :** `type: feature` · `P1 - High` · `M` · `Milestone: M2`

### User Story

En tant que **drone**, je veux **observer mon environnement et mon propre état**, afin que **je puisse décider de mes actions**.

### Critères d'acceptation

- [ ] `Look` renvoie le contenu des cases visibles dans l'ordre de numérotation du sujet ; la portée vaut 1 au niveau 1 et augmente d'une rangée par niveau ; cases séparées par des virgules, objets d'une case par des espaces, joueurs inclus, rebouclage torique pris en compte.
- [ ] `Inventory` renvoie les 7 ressources avec leurs quantités et le temps de vie restant.
- [ ] `Connect_nbr` renvoie le nombre de slots libres de l'équipe (immédiat).

### Notes

- Format `Look` : `[case0, case1, ...]` (séparateur virgule, avec ou sans espace).

### Depends on

- #1, #4, #7

---

## #10 — [server] Broadcast and sound propagation

**Labels :** `type: feature` · `P2 - Medium` · `M` · `Milestone: M2`

### User Story

En tant que **drone**, je veux **émettre un message audible par tous**, afin que **les autres perçoivent le message et sa direction d'origine sans connaître l'émetteur**.

### Critères d'acceptation

- [ ] `Broadcast text` répond `ok` à l'émetteur.
- [ ] Tous les drones reçoivent `message K, text`, où `K` est la case de provenance (1 à 8 selon l'orientation du récepteur, 0 si même case).
- [ ] `K` est calculé sur le plus court chemin du monde torique.

### Notes

- L'émetteur reste anonyme (cf. schéma de numérotation du son dans le sujet).

### Depends on

- #1, #4, #7

---

## #11 — [server] Reproduction and ejection

**Labels :** `type: feature` · `P2 - Medium` · `M` · `Milestone: M2`

### User Story

En tant que **drone**, je veux **pondre des œufs et chasser les autres de ma case**, afin que **mon équipe s'agrandisse et que je puisse disputer un emplacement**.

### Critères d'acceptation

- [ ] `Fork` pond un œuf sur la case du drone et ouvre un nouveau slot d'équipe → `ok`.
- [ ] `Eject` pousse tous les autres drones de la case d'une case dans la direction regardée → `ok` si au moins un a été poussé, sinon `ko`.
- [ ] Les drones éjectés reçoivent `eject: K` (direction de provenance).
- [ ] L'éjection détruit les œufs présents sur la case.

### Notes

- La ponte autorise une nouvelle connexion sur l'équipe (cf. #6).

### Depends on

- #4, #6, #7

---

## #12 — [server] Elevation ritual

**Labels :** `type: feature` · `P1 - High` · `L` · `Milestone: M2`

### User Story

En tant que **drone**, je veux **participer à un rituel d'élévation**, afin que **je monte de niveau et progresse vers la victoire**.

### Critères d'acceptation

- [ ] L'incantation vérifie au **démarrage** le nombre de joueurs de même niveau et les minerais requis sur la case (table du sujet, paliers 1→2 … 7→8) ; si non rempli → `ko`.
- [ ] Les participants sont **gelés** pendant le rituel (`300/f`) et ne traitent aucune autre action.
- [ ] Les prérequis sont **revérifiés à la fin** ; en cas d'échec, le rituel échoue.
- [ ] En cas de succès, les minerais sont retirés de la case et tous les participants reçoivent `Current level: k` et montent de niveau.

### Notes

- Les participants n'ont pas besoin d'être de la même équipe, seulement du même niveau.

### Depends on

- #1, #4, #7

---

## #13 — [server] End of game and victory condition

**Labels :** `type: feature` · `P1 - High` · `S` · `Milestone: M2`

### User Story

En tant que **organisateur de la partie**, je veux **détecter la victoire d'une équipe**, afin que **la partie se termine proprement et que le résultat soit communiqué**.

### Critères d'acceptation

- [ ] La condition « au moins 6 joueurs au niveau 8 dans une même équipe » est évaluée après chaque élévation.
- [ ] Au déclenchement, l'équipe gagnante est identifiée et la fin de partie est émise.

### Notes

- La fin de partie est notifiée au GUI (`seg`, cf. #17).

### Depends on

- #12

---
---

# Milestone 3 — Protocole GUI

## #14 — [server] GUI queries: map and teams

**Labels :** `type: feature` · `P1 - High` · `M` · `Milestone: M3`

### User Story

En tant que **client graphique**, je veux **interroger la carte et les équipes**, afin que **je puisse dessiner le monde et lister les équipes**.

### Critères d'acceptation

- [ ] Taille de carte (`msz`), contenu d'une case (`bct X Y`) et de toute la carte (`mct`) sont renvoyés au bon format.
- [ ] Les noms d'équipes sont renvoyés (`tna`).
- [ ] Une commande GUI inconnue renvoie `suc` ; un paramètre invalide renvoie `sbp`.

### Notes

- Note d'avancement : les définitions de messages GUI existent déjà ; la logique de réponse est à écrire.
- Format exact : voir le document « ZAPPY - GUI PROTOCOL ».

### Depends on

- #1, #6

---

## #15 — [server] GUI queries: players and time control

**Labels :** `type: feature` · `P1 - High` · `M` · `Milestone: M3`

### User Story

En tant que **client graphique**, je veux **interroger l'état des joueurs et régler le rythme**, afin que **je puisse afficher chaque drone et accélérer/ralentir la partie**.

### Critères d'acceptation

- [ ] Position/orientation (`ppo #n`), niveau (`plv #n`) et inventaire (`pin #n`) d'un joueur sont renvoyés.
- [ ] L'unité de temps est lisible (`sgt`) et modifiable à chaud (`sst T`), le nouveau rythme s'appliquant aux actions à venir.

### Notes

- La modification du rythme impacte les durées calculées via `f` (cf. #3).

### Depends on

- #3, #7

---

## #16 — [server] GUI events: players and resources

**Labels :** `type: feature` · `P2 - Medium` · `M` · `Milestone: M3`

### User Story

En tant que **client graphique**, je veux **recevoir en direct les événements de joueurs et de ressources**, afin que **l'affichage des drones et du sol reste synchronisé**.

### Critères d'acceptation

- [ ] Connexion d'un joueur (`pnw`), changements de position/niveau/inventaire (`ppo`/`plv`/`pin`) et mort (`pdi`) sont poussés.
- [ ] Collecte (`pgt`) et dépôt (`pdr`) de ressources sont poussés.
- [ ] Un réapprovisionnement ne pousse que les **cases modifiées** (`bct`).

### Notes

- Optimisation demandée : éviter les notifications redondantes ; les actions joueur utilisent des événements dédiés plutôt qu'un push global.

### Depends on

- #2, #7, #8, #14

---

## #17 — [server] GUI events: game, eggs and system

**Labels :** `type: feature` · `P2 - Medium` · `M` · `Milestone: M3`

### User Story

En tant que **client graphique**, je veux **recevoir en direct les événements de jeu, d'œufs et système**, afin que **je puisse animer broadcasts, éjections, incantations, œufs et fin de partie**.

### Critères d'acceptation

- [ ] Broadcast (`pbc`) et éjection (`pex`) sont poussés.
- [ ] Début et fin d'incantation (`pic`/`pie`) sont poussés.
- [ ] Événements d'œufs poussés : ponte (`pfk`, `enw`), connexion sur œuf (`ebo`), destruction (`edi`).
- [ ] Message serveur (`smg`) et fin de partie avec gagnant (`seg`) sont poussés.

### Notes

- Chaque famille d'événement est liée à sa mécanique (#10, #11, #12, #13, #6).

### Depends on

- #14

---
