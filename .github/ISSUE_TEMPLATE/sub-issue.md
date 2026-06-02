---
name: Sub-issue
about: Découper une user story parente en tâche unitaire et actionnable
title: "[<module>] <short task title in English>"
labels: []
assignees: []
---

<!--
Le titre de l'issue DOIT être rédigé en anglais (le corps reste en français).
Format : "[<module>] <short task title in English>".

Labels à poser manuellement sur le board (exemples) :
  - type:        type: infra, type: cmake, type: feature, type: bug, …
  - priorité:    P0 - Critical, P1 - High, P2 - Medium, P3 - Low
  - taille:      XS, S, M, L, XL
  - milestone:   Milestone: M1, M2, …
Supprime ce bloc une fois l'issue rédigée.
-->

## Parent

<!--
Issue parente (user story) à laquelle cette tâche se rattache.
Référencer par numéro avec « # » pour que GitHub crée le lien automatiquement.
-->

- Parent: #<numéro de la user story parente>

## Objectif

<!-- Une à deux phrases : ce que cette tâche réalise, et son périmètre exact. -->

<description concise de la tâche>

## Critères d'acceptation

<!-- Chaque critère doit être vérifiable objectivement (oui / non). -->

- [ ] <critère 1>
- [ ] <critère 2>

## Notes

<!-- Contraintes techniques, pièges à éviter, références. Optionnel. -->

- <note 1>

## Depends on

<!--
Lister les issues bloquantes en les référençant par leur numéro avec « # »
(GitHub crée alors le lien automatiquement). Une référence par ligne.
Mettre « — » s'il n'y a aucune dépendance.
-->

- #<numéro de l'issue bloquante>
