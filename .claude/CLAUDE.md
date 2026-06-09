# Project Rules — Raytracer

Ce fichier définit les règles de codage et d'architecture du projet. Il est
chargé automatiquement par Claude Code à chaque session sur ce repo et doit
être respecté à la lettre.

## Règle méta — comment ce fichier évolue

À chaque fois que l'utilisateur remonte un problème sur la manière de coder,
sur l'architecture, sur le style, ou sur un anti-pattern repéré dans le code,
**ce retour doit être transformé en règle numérotée et ajoutée à ce fichier**.

Principes :

- Les règles sont **permanentes** et **numérotées de façon stable**
  (`R1`, `R2`, …). On ne renumérote jamais une règle existante : les
  références (`cf. R3`) doivent rester valides dans le temps.
- On ne supprime une règle qu'avec une décision explicite de l'utilisateur.
- Si une règle est révisée, on garde le même numéro et on note la
  modification.
- Avant d'ajouter une nouvelle règle, vérifier qu'elle ne fait pas doublon
  avec une règle existante — dans ce cas, **étendre** la règle existante
  plutôt qu'en créer une nouvelle.

### Format d'une règle

```
### R<n> — <titre court>

**Règle :** <énoncé impératif en une phrase>.

**Pourquoi :** <raison / contexte>.

**À appliquer :** <où et quand la règle s'active : fichiers, situations>.

**Exemple interdit :** <snippet>
**Exemple correct :** <snippet>
```

Les sections `Exemple interdit` / `Exemple correct` sont optionnelles mais
recommandées dès qu'un cas concret aide à lever l'ambiguïté.

### Comment ajouter une nouvelle règle

1. Prendre le prochain numéro libre : si la dernière règle est `R7`, la
   nouvelle est `R8`.
2. Suivre le gabarit ci-dessus.
3. L'insérer à la fin de la section **Règles** ci-dessous.
4. Ne **pas** renuméroter les règles existantes.

---

## Règles

### R1 — Séparation stricte `.hpp` / `.cpp`

**Règle :** un fichier `.hpp` ne contient que des **déclarations** (classes,
signatures de méthodes, `enum`, `struct`, `using`, déclarations de templates,
forward declarations). **Aucune logique métier, aucun corps de méthode** ne
doit apparaître dans un `.hpp`. Toute définition va dans le `.cpp`
correspondant.

**Pourquoi :** garantir une séparation claire interface / implémentation,
réduire les temps de compilation, éviter les dépendances transitives via les
headers, et faciliter la lecture (un `.hpp` doit pouvoir être survolé en
quelques secondes pour comprendre l'API d'une classe).

**À appliquer :** tous les fichiers `.hpp` du projet, sans exception
fonctionnelle.

**Exceptions techniques limitées et justifiées :**

- Templates qui *doivent* techniquement être définis dans le header (ou dans
  un `.tpp` inclus en fin de header).
- `constexpr` lorsque la définition est requise à la compilation.
- Méthodes triviales `= default` / `= delete`.

Tout le reste — y compris les getters/setters d'une ligne — va dans le
`.cpp`.

**Exemple interdit (`Foo.hpp`) :**

```cpp
class Foo {
 public:
    int bar() { return 42; }                  // INTERDIT : corps dans le .hpp
    void setName(const std::string& n) {      // INTERDIT
        name_ = n;
    }
 private:
    std::string name_;
};
```

**Exemple correct :**

```cpp
// Foo.hpp
class Foo {
 public:
    int bar();
    void setName(const std::string& n);
 private:
    std::string name_;
};
```

```cpp
// Foo.cpp
#include "Foo.hpp"

int Foo::bar() { return 42; }
void Foo::setName(const std::string& n) { name_ = n; }
```

### R2 — Interfaces purement virtuelles

**Règle :** une **interface** (classe destinée à être uniquement implémentée
par d'autres, jamais instanciée) doit n'exposer que des **méthodes purement
virtuelles** (`= 0`) et un destructeur virtuel `= default`. Aucune méthode
d'interface ne doit avoir de corps `{}`, même vide.

**Pourquoi :** une interface décrit un contrat, pas un comportement. Fournir
une implémentation par défaut (`virtual void render() {}`) brouille la
distinction interface / classe abstraite et masque les oublis
d'implémentation côté classes dérivées.

**À appliquer :** toute classe dont le rôle est d'être une interface
(typiquement préfixée `I`, par exemple `IRenderer`, `IPrimitive`,
`ILightSource`). Les classes abstraites *partiellement* implémentées ne sont
pas des interfaces et ne sont pas concernées par cette règle.

**Exemple interdit :**

```cpp
class IRenderer {
 public:
    virtual ~IRenderer() = default;
    virtual void render() {}                  // INTERDIT : corps {}
    virtual void setQuality(int q) {}         // INTERDIT
};
```

**Exemple correct :**

```cpp
// IRenderer.hpp
class IRenderer {
 public:
    virtual ~IRenderer() = default;
    virtual void render() = 0;
    virtual void setQuality(int q) = 0;
};
```

Note : le destructeur virtuel par défaut (`= default`) est autorisé et
recommandé — c'est la seule "implémentation" tolérée dans une interface.

### R3 — Aucun co-auteur ni mention d'assistant dans les commits

**Règle :** les messages de commit ne doivent contenir **aucun trailer
`Co-Authored-By:`**, **aucune mention d'un assistant IA** (Claude, Copilot,
ChatGPT, Cursor, etc.), **aucun lien vers un outil d'IA**, et **aucun
emoji robot / "Generated with"**. Le sujet, le corps et les trailers du
commit doivent décrire uniquement *ce que* le commit change et *pourquoi*.

**Pourquoi :** la paternité d'un commit appartient à l'auteur humain qui
prend la responsabilité du code. L'outillage utilisé pour produire le diff
(IA, IDE, snippets) est un détail privé qui ne doit pas polluer
l'historique git ni le repo mirror Epitech. Les Co-Authored-By légitimes
(pair-programming humain) ne sont pas l'usage qu'on a ici, donc on
l'interdit pour éviter toute ambiguïté.

**À appliquer :** absolument tous les commits du projet, y compris les
commits intermédiaires d'une PR (squash ou non). S'applique aussi à toute
réécriture d'historique : si un commit existant contient une de ces
mentions, il doit être réécrit avant push.

**Exemple interdit :**

```
chore: gitignore the subject PDF

Co-Authored-By: Claude Opus 4.7 (1M context) <noreply@anthropic.com>
```

```
feat: add Vector3 utilities

🤖 Generated with [Claude Code](https://claude.com/claude-code)
```

**Exemple correct :**

```
chore: gitignore the subject PDF
```

```
feat: add Vector3 utilities

Implements addition, scaling and dot product. Used by the camera
ray-generation step.
```

### R4 — Indexation mémoire à chaque découverte de fichier ou de système

**Règle :** dès qu'un fichier, une interface, un pattern CMake ou un système est **lu ou découvert** au cours d'une session, son contenu essentiel (API, signatures, conventions, emplacement) doit être **indexé ou mis à jour dans les fichiers mémoire** avant la fin de la session.

**Pourquoi :** éviter de re-lire et re-explorer les mêmes fichiers à chaque nouvelle conversation, ce qui consomme des crédits inutilement. Chaque re-découverte est un coût payé deux fois.

**À appliquer :** systématiquement après avoir lu un `.hpp`, `.cpp`, `CMakeLists.txt`, fixture de test, interface ou après avoir compris un pattern non encore documenté en mémoire.

**Exemple interdit :** lire `ITransformation.hpp` pour utiliser `apply()` / `applyToNormal()` sans en laisser de trace en mémoire.

**Exemple correct :** après avoir lu `ITransformation.hpp`, mettre à jour `project_component_structure.md` avec la signature de ses méthodes, puis mettre à jour `MEMORY.md`.

### R5 — Pas de préfixe `k` pour les constantes

**Règle :** les constantes (`constexpr`, `const`, valeurs d'`enum`) ne doivent **pas** être préfixées par `k` (style Google). Utiliser un nom direct en `camelCase` ou en `UPPER_SNAKE_CASE` selon le contexte.

**Pourquoi :** le préfixe `k` est une convention Google qui n'apporte rien au-delà de ce que `constexpr` / `const` / `enum` expriment déjà au type system. Elle pollue la lecture sans ajouter d'information. Le projet utilise déjà des constantes sans préfixe (`epsilon` dans `Constants.hpp`, `DEFAULT_FOV` dans `PerspectiveCamera.hpp`).

**À appliquer :** toute nouvelle constante. Les constantes existantes en `kFoo` doivent être renommées dès qu'on touche au fichier.

**Exemple interdit :**

```cpp
constexpr double kShadowRayTMin = 0.001;
constexpr int kMaxDepth = 10;
constexpr double kEpsilon = 1e-9;
```

**Exemple correct :**

```cpp
constexpr double shadowRayTMin = 0.001;
constexpr int maxDepth = 10;
constexpr double epsilon = 1e-9;

// UPPER_SNAKE_CASE accepté pour les constantes "publiques" exposées
// dans un header :
constexpr double DEFAULT_FOV = 90.0;
```

### R6 — Pas d'abréviation dans les identifiants

**Règle :** les identifiants publics (méthodes, fonctions, classes, variables
membres exposées) doivent utiliser des **noms complets et explicites**, jamais
des abréviations — même quand l'abréviation est une convention répandue dans
le domaine (ex. PBRT, mitsuba, etc.).

**Pourquoi :** une abréviation idiomatique pour un expert du domaine est
opaque pour tout autre lecteur. Le code est lu plus souvent qu'écrit, et le
public du projet (jury, futurs collaborateurs Epitech) ne partage pas
forcément le bagage théorique sous-jacent. Le coût supplémentaire à
l'écriture est négligeable, le gain en lisibilité est durable.

**À appliquer :** toute nouvelle déclaration et toute déclaration touchée
lors d'un refactoring. Les abréviations établies pour les types fondamentaux
de la STL (`std::ptr`, `std::ref`) ne sont pas concernées — la règle vise les
identifiants **du projet**.

**Exemple interdit :**

```cpp
// Référence PBRT : Li = "incoming radiance"
virtual math::Color Li(const math::Ray& ray,
                       const scene::Scene& scene, int depth) = 0;

// Idem pour des abréviations "métier"
double calcBRDF(const Vector3D& wi, const Vector3D& wo);  // wi, wo, BRDF
int computeNbHits();                                       // Nb
void initCfg();                                            // Cfg
```

**Exemple correct :**

```cpp
virtual math::Color computeRadiance(const math::Ray& ray,
                                    const scene::Scene& scene,
                                    int depth) = 0;

double computeReflectance(const Vector3D& incomingDirection,
                          const Vector3D& outgoingDirection);
int computeHitCount();
void initConfiguration();
```

### R7 — Underscore en suffixe pour les membres privés

**Règle :** les variables membres **privées** d'une classe doivent porter
un underscore **en suffixe** (`name_`, `position_`, `mode_`), jamais en
préfixe (`_name`, `_position`). Les membres publics n'ont pas
d'underscore du tout.

**Pourquoi :** le préfixe `_identifier` est réservé par le standard C++ à
certains contextes (au niveau global) et reste ambigu avec les
conventions C (`__internal`, `_t`). Le suffixe `name_` est la convention
historiquement utilisée par Google C++ Style et adoptée dans ce projet
(`topColor_`, `bottomColor_`, `color_`, `triangles_`, etc.). Mélanger les
deux casse la lecture et trahit un code « collé » d'un autre projet.

**À appliquer :** toute nouvelle déclaration de membre privé, et toute
déclaration existante touchée lors d'un refactoring. Les membres publics
de types « POD-like » (ex. `Vector3D::x`, `Color::r`, `PointLight::position`)
restent sans underscore puisqu'ils font partie de l'API publique.

**Exemple interdit :**

```cpp
class MonoThreadRenderer : public IRenderer {
 private:
    std::function<void(double)> _progressCallback;   // préfixe : INTERDIT
    bool _environmentLighting{false};                // INTERDIT
};
```

**Exemple correct :**

```cpp
class MonoThreadRenderer : public IRenderer {
 private:
    std::function<void(double)> progressCallback_;
    LightingMode lightingMode_{LightingMode::Whitted};
};
```

### R8 — Logger : instance par module, pas de macros, pas de debug-only

**Règle :** toute classe qui veut logger doit posséder son propre membre
`raytracer::common::Logger logger_{"NomDuModule"};` et appeler les
méthodes d'instance (`logger_.info(...)`, `.warn`, `.error`, `.debug`,
`.trace`, `.scope`). Les anciennes macros `RT_LOG_*` et l'API
statique `Logger::log(...)` sont **interdites** et n'existent plus.
Le filtrage compile-time se fait **uniquement** via la variable CMake
`LOG_LEVEL` (traduite en `RT_LOG_BUILD_LEVEL`) ; on n'utilise plus
`#ifdef NDEBUG` pour décider si un log existe ou pas.

**Pourquoi :** le Logger est une *facility applicative*, pas un outil
de debug. Il doit fonctionner en release et produire les logs utiles à
toute application (démarrage, fin, erreurs, durée). Le nom de module
appartient à la classe et est paié une seule fois à la construction :
le passer en string à chaque appel est redondant et fragile (typo,
oubli). Les méthodes (vs macros) gagnent typage, auto-complete, et un
debugger qui voit les appels — le filtrage compile-time est obtenu via
`if constexpr (isCompiledIn(...))`, qui supprime le code aussi
proprement qu'un `#ifdef`. Enfin, le défaut runtime `Info` garantit
qu'une exécution normale n'affiche aucun bruit `Debug`.

**À appliquer :** toute nouvelle classe qui log, et toute classe
existante touchée lors d'un refactoring. Pour un module qui logge
ponctuellement depuis une fonction libre, on accepte un
`Logger("ModuleName")` local — mais le pattern par défaut reste le
membre d'instance. Pour mesurer une durée : `auto timer =
logger_.scope("label");` (RAII, niveau `Info` par défaut).

**Exemple interdit :**

```cpp
// API statique disparue : INTERDIT
raytracer::common::Logger::log(LogLevel::Info, "Renderer", "hello");

// Macros disparues : INTERDIT
RT_LOG_INFO("Renderer", "starting render " << w << 'x' << h);

// #ifdef NDEBUG pour conditionner un log : INTERDIT
#ifndef NDEBUG
  std::cerr << "[debug] pool=" << n << '\n';
#endif

// Logger statique recréé à chaque appel : INTERDIT
void Foo::run() {
    raytracer::common::Logger("Foo").info("local instance throwaway");
}
```

**Exemple correct :**

```cpp
class RaytracerRenderer : public IRenderer {
 private:
    raytracer::common::Logger logger_{"Renderer"};

 public:
    components::Image render(...) {
        auto timer = logger_.scope("render()");
        logger_.info("starting render ", w, 'x', h,
                     ", samples=", samples);
        if (tiles.empty()) {
            logger_.warn("no tiles produced");
            return image;
        }
        logger_.debug("ThreadPool spawned ", n, " worker(s)");
        // …
    }
};
```

**Configuration de niveau :**

- Compile-time : `cmake -B build -DLOG_LEVEL=info` strippe `debug` et
  `trace` du binaire. Défaut : `info` en `Release`, `trace` ailleurs.
- Runtime : `RT_LOG_LEVEL=debug ./raytracer …` ou
  `Logger::setLevel(LogLevel::Debug)`. Défaut : `Info`.

### R9 — Le shading vit dans l'intégrateur, jamais dans le renderer

**Règle :** toute la logique de shading (intersection → couleur) doit
vivre dans une classe implémentant `IIntegrator` (typiquement
`PathIntegrator`). Un renderer (`MonoThreadRenderer`,
`RaytracerRenderer`, futurs ports GPU) n'a qu'**une seule** raison de
toucher au shading : appeler
`config.integrator->computeRadiance(ray, scene, settings.maxDepth)`
pour chaque rayon primaire qu'il génère. **Aucun** `castRay`,
`shade`, `shadeWireframe`, `shadeMaterialPreview`, `shadeRendered`,
helper Lambert, ni dispatch de `ViewportMode` ne doit apparaître dans
un fichier de renderer.

Si `config.integrator == nullptr`, le renderer doit **lever une
exception** au début de `render()` — surtout pas se rabattre sur une
implémentation locale du shading, ce qui rouvrirait la porte à la
divergence.

**Pourquoi :** le shading est la seule logique métier *partagée* entre
le mono-thread et le multi-thread. Le dupliquer entre deux renderers
(comme c'était le cas avant) garantit la dérive : un fix appliqué d'un
côté ne suit pas de l'autre, le rendu multi-thread sort des couleurs
légèrement différentes du mono-thread, et la régression est invisible
jusqu'à comparaison pixel-à-pixel. En centralisant côté `IIntegrator`,
les deux renderers partagent **exactement** la même chaîne et le test
`RaytracerRendererTest.ProducesSameImageAsMonoThread` peut être
strictement `EXPECT_TRUE(pixelsEqual(...))`. Bonus : ajouter un futur
intégrateur (BDPT, MLT, GPU compute) revient à écrire une seule classe,
pas à modifier deux renderers.

**À appliquer :** toute nouvelle implémentation de renderer, toute
modification de renderer existant. Quand on touche un renderer et que
l'IDE complète vers une fonction `shade*` ou `castRay`, c'est le signe
qu'on est dans la mauvaise classe — passer côté `IIntegrator`.

**Exemple interdit :**

```cpp
// dans RaytracerRenderer.cpp — INTERDIT
namespace {
math::Color castRay(const math::Ray& ray,
                    const scene::Scene& scene, int depth,
                    bool isPrimary) {
    // … logique de shading dupliquée …
}
}

components::Image RaytracerRenderer::render(...) {
    for (const Tile& tile : tiles) {
        // …
        const math::Color color =
            castRay(ray, scene, settings.maxDepth, true);  // INTERDIT
        image.setPixel(x, y, color);
    }
}
```

```cpp
// dans MonoThreadRenderer.cpp — fallback silencieux INTERDIT
components::Image MonoThreadRenderer::render(const RendererConfig& config,
                                              const Frame& frame) {
    auto integrator = config.integrator
        ? config.integrator
        : std::make_shared<PathIntegrator>();  // INTERDIT (fallback silencieux)
    // …
}
```

**Exemple correct :**

```cpp
// PathIntegrator.cpp — TOUTE la logique de shading vit ici
math::Color PathIntegrator::computeRadiance(const math::Ray& ray,
                                            const scene::Scene& scene,
                                            int depth) {
    return castRay(ray, scene, depth, /*isPrimary=*/true);
}

// Renderer — appel unique, pas de logique métier
components::Image RaytracerRenderer::render(const RendererConfig& config,
                                             const Frame& frame) {
    if (!config.integrator) {
        throw std::invalid_argument(
            "RaytracerRenderer::render requires RendererConfig.integrator");
    }
    IIntegrator& integrator = *config.integrator;
    // … boucle pixels …
    const math::Color color =
        integrator.computeRadiance(ray, scene, settings.maxDepth);
    image.setPixel(x, y, color);
}

// Application — wiring explicite
const RendererConfig config{
    .scene = scene,
    .settings = settings,
    .integrator = std::make_shared<PathIntegrator>()};
```

### R10 — Aucun commentaire dans le corps des fonctions

**Règle :** le corps d'une fonction, méthode, ou lambda **ne doit
contenir aucun commentaire** (`//`, `/* … */`, ni paragraphe explicatif,
ni hint type `/*paramName=*/`). La seule documentation autorisée vit
**au-dessus** des déclarations dans les `.hpp`, sous forme **Doxygen**
(`/** … */` avec `@brief`, `@param`, `@returns`).

**Pourquoi :** un commentaire dans une fonction décrit *ce que fait* le
code à côté — information que le code lui-même exprime déjà quand les
identifiants sont bien nommés (cf. R6). Quand le code change, le
commentaire pourrit silencieusement et finit par mentir. La doc Doxygen
au-dessus des déclarations a deux avantages décisifs : (1) elle est
extraite par l'outillage (clang-doc, Doxygen, IDE tooltips) et apparaît
au point d'appel sans avoir à ouvrir le `.cpp` ; (2) elle décrit le
**contrat** (entrées, sorties, invariants) qui change beaucoup moins
souvent que l'implémentation.

**À appliquer :** toute nouvelle fonction et toute fonction touchée
lors d'un refactoring. Si on ressent le besoin d'expliquer une portion
de logique dans le corps, c'est le signe qu'il faut soit (a) extraire
une fonction nommée, soit (b) renommer une variable. Le commentaire
n'est jamais la bonne réponse.

**Exceptions techniques limitées :**

- **Directives de tooling** (`// NOLINTNEXTLINE(rule)`,
  `// clang-format off`, `// NOLINTBEGIN/END`) — ce ne sont pas des
  commentaires narratifs, c'est de la configuration adressée à un outil.
- **Bannière d'en-tête de fichier** (le bloc `EPITECH PROJECT, 2026 …`)
  imposée par la norme Epitech.
- **Doxygen au-dessus d'une déclaration interne** (helper privé, free
  function locale en namespace anonyme) — autorisé si le helper est
  réutilisé dans le fichier et que sa sémantique mérite un contrat
  explicite. À éviter pour les helpers utilisés une seule fois.

**Exemple interdit :**

```cpp
math::Color shadeRendered(const math::Ray& inRay, const HitRecord& record,
                          const scene::Scene& scene, int depth) {
    // Compute the unit normal first so the cosine factor is correct.
    const Vector3D unitNormal = unitShadingNormal(record);
    // Offset the shading point along the normal to avoid self-shadowing.
    const Vector3D shadingPoint =
        record.point + (unitNormal * shadowRayEpsilon);
    // Iterate over the lights and sum the Lambert contribution.
    Color directLighting(0, 0, 0);
    for (const auto& light : scene.getLights()) {
        directLighting = directLighting +
                         lambertContributionFromLight(*light, shadingPoint,
                                                      unitNormal,
                                                      record.material->diffuseAlbedo(),
                                                      scene);
    }
    return directLighting;
}

// hint paramètre — INTERDIT
castRay(scattered, scene, depth - 1, /*isPrimary=*/false);
```

**Exemple correct :**

```cpp
/**
 * @brief Whitted-style direct lighting + indirect bounce for the
 *        `Rendered` viewport mode.
 *
 * @param[in] inRay   The incoming ray hitting @p record.
 * @param[in] record  Hit record produced by the scene intersection.
 * @param[in] scene   Scene queried for lights, occlusion, and background.
 * @param[in] depth   Remaining bounce budget for the indirect scatter.
 * @returns Linear-space radiance carried back along @p inRay.
 */
math::Color shadeRendered(const math::Ray& inRay,
                          const math::HitRecord& record,
                          const scene::Scene& scene,
                          int depth);
```

```cpp
math::Color shadeRendered(const math::Ray& inRay,
                          const math::HitRecord& record,
                          const scene::Scene& scene,
                          int depth) {
    const Vector3D unitNormal = unitShadingNormal(record);
    const Vector3D shadingPoint =
        record.point + (unitNormal * shadowRayEpsilon);

    Color directLighting(0, 0, 0);
    for (const auto& light : scene.getLights()) {
        directLighting =
            directLighting +
            lambertContributionFromLight(*light, shadingPoint, unitNormal,
                                         record.material->diffuseAlbedo(),
                                         scene);
    }
    return directLighting;
}

castRay(scattered, scene, depth - 1, false);
```

### R11 — Nom de fichier = nom PascalCase de la classe principale

**Règle :** un fichier `.hpp` / `.cpp` doit porter le **nom exact, en
`PascalCase`, de la classe (ou de l'entité) principale qu'il
déclare/définit**. Pas de `snake_case`, pas de `lowercase`, pas de
`kebab-case`.

**Pourquoi :** tout le projet suit cette convention
(`RaytracerRenderer.cpp`, `ArgsParser.hpp`, `IImageWriter.hpp`,
`WhittedIntegrator.cpp`, `PerspectiveCamera.cpp`…). Un fichier
`status_overlay.hpp` ou `viewport_runner.cpp` au milieu casse la
cohérence, complique la navigation (on cherche la classe `StatusOverlay`
et on doit deviner le casing du fichier), et trahit du code « collé »
d'un autre projet. Le nom de fichier doit pouvoir se déduire
mécaniquement du nom de la classe, et inversement.

**À appliquer :** tout nouveau fichier. Un fichier existant dont le nom
ne respecte pas la règle doit être renommé dès qu'on le touche lors d'un
refactoring. Le préfixe `I` des interfaces fait partie du nom de classe
et donc du nom de fichier (`IRenderer.hpp`). Pour un fichier qui ne
contient pas de classe (free functions, helpers), utiliser le
`PascalCase` du concept qu'il regroupe.

**Exemple interdit :**

```
status_overlay.hpp        // classe StatusOverlay
viewport_runner.cpp       // classe ViewportRunner
framebuffer.hpp           // classe Framebuffer
render-settings.hpp       // classe RenderSettings
```

**Exemple correct :**

```
StatusOverlay.hpp
ViewportRunner.cpp
Framebuffer.hpp
RenderSettings.hpp
```

### R12 — Le code Python doit passer pylint (≥ 8.0) et flake8 sans erreur

**Règle :** tout fichier Python produit ou modifié doit passer
`flake8` sans avertissement et `pylint` avec un score ≥ 8.0.
Aucun `# pylint: disable` ni `# noqa` ne doit être ajouté pour
faire taire un avertissement légitime — la bonne réponse est de
corriger le code.

**Pourquoi :** la CI lint (`python-lint`) vérifie ces deux outils
sur les fichiers modifiés à chaque PR. Un `disable` en dur déplace
le problème sans le résoudre, masque les vrais défauts et pollue
l'historique. Les seuls silences acceptables sont ceux qui couvrent
une contrainte technique documentée (ex. `redefined-outer-name` sur
les fixtures pytest quand le nom de fixture est intentionnellement
identique à un nom externe — et encore, la préférence va à renommer).

**À appliquer :** tous les fichiers `.py` du projet, sans exception.

**Exemple interdit :**

```python
# pylint: disable=redefined-outer-name   # INTERDIT : renommer la variable
# pylint: disable=protected-access       # INTERDIT : revoir la conception
x=1  # noqa: E225                        # INTERDIT : corriger l'espacement
```

**Exemple correct :**

```python
# fixture pytest : nom identique voulu — seul cas où disable est toléré
# pylint: disable=redefined-outer-name
@pytest.fixture
def client():
    return FakeClient()
```

### R13 — Gestion d'erreur : exceptions custom dans `Exceptions/`, jamais de `std::` brut

**Règle :** toute erreur levée par du code métier doit l'être via une **classe
d'exception custom** définie dans un dossier `Exceptions/` du module concerné.
Il est **interdit de `throw` directement une exception de la bibliothèque
standard** (`std::invalid_argument`, `std::runtime_error`, `std::out_of_range`,
`std::logic_error`, etc.). Chaque module possède une **exception racine** qui
hérite de `std::runtime_error`, et des **sous-classes spécifiques** par cas
d'erreur. Avant d'en créer une, vérifier si elle existe déjà — sinon
l'**étendre** plutôt que d'en multiplier. Les messages utilisateur vont dans
`common/Error/Messages/<Module>Messages.hpp` (namespace
`zappy::error::messages`, `inline constexpr const char *`), **jamais** en dur
dans le `throw`.

**Pourquoi :** une exception `std::` générique levée depuis le métier ne porte
aucune information de domaine : l'appelant ne peut pas distinguer « dimensions
de carte invalides » d'une autre `std::invalid_argument` venue d'ailleurs, et
ne peut pas catcher *la famille* d'erreurs d'un module en un seul `catch`. La
hiérarchie custom (racine `…Exception` + filles) donne un `catch` par famille,
des messages cohérents et centralisés (testables, traduisibles, sans
duplication), et empêche une exception std de fuiter à travers une frontière de
module. C'est déjà le pattern du projet (`ParserException` +
`ParserMessages.hpp`) ; on le généralise.

**À appliquer :** tout code qui lève une erreur, dans tous les modules. Quand on
s'apprête à écrire `throw std::…`, c'est le signal qu'il faut une classe dans
le `Exceptions/` du module (la créer si absente). Le `Exceptions/` et le
`<Module>Messages.hpp` sont ajoutés à la lib CMake du module.

**Exemple interdit :**

```cpp
// Map.cpp — INTERDIT : std brut + message en dur
Map::Map(int width, int height) : width_(width), height_(height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Map dimensions must be strictly positive");
    }
    // …
}
```

**Exemple correct :**

```cpp
// server/App/World/Exceptions/WorldException.hpp
namespace zappy::world {
class WorldException : public std::runtime_error {
 public:
    explicit WorldException(const std::string &message);
};
class InvalidMapDimensionsException : public WorldException {
 public:
    InvalidMapDimensionsException(int width, int height);
};
}  // namespace zappy::world
```

```cpp
// Map.cpp — métier : on lève l'exception de domaine
if (width <= 0 || height <= 0) {
    throw InvalidMapDimensionsException(width, height);
}
```

### R14 — Texte Git/GitHub en anglais et template de PR respectée

**Règle :** tout texte qui atterrit dans Git/GitHub — **messages de commit**,
**titres de PR** et **descriptions de PR** — est rédigé en **anglais**, quelle
que soit la langue de la conversation avec l'utilisateur. Toute description de PR
doit suivre **exactement** la structure de `.github/PULL_REQUEST_TEMPLATE.md` :
**toutes** les sections, **dans l'ordre**, avec les cases à cocher remplies
honnêtement (cocher uniquement ce qui est vrai). Si la template évolue, c'est la
version courante du fichier qui fait foi.

**Pourquoi :** l'historique git et les PR sont publics (jury Epitech, repo
mirror, futurs collaborateurs) et **déjà 100 % en anglais** (cf. `git log`) ; y
injecter du français casse la cohérence et trahit un texte produit hors process.
La template existe pour rendre les PR comparables et relisables : la contourner
fait sauter les sections (Summary / Changes / Type of Change / Testing /
Checklist) qui structurent la revue. La langue du chat n'a aucune incidence sur
ce qui est écrit dans le repo.

**À appliquer :** chaque commit, chaque titre et chaque description de PR. Avant
d'ouvrir ou de mettre à jour une PR, **lire** `.github/PULL_REQUEST_TEMPLATE.md`
et calquer la description dessus.

**Périmètre :** la règle vise le texte **Git/GitHub** uniquement. Les **docs
d'architecture sous `doc/`** restent en **français**, comme l'existant
(`ARCHITECTURE.cRPC.md`, `SERVER_ISSUES.md`…) — ne pas les traduire.

**Exemple interdit (description de PR) :**

```md
## Résumé
Cette PR ajoute le monde torique et le scheduler.

## Modifications
- ...
```

**Exemple correct (description de PR) :**

```md
## Summary
Adds the toroidal world model and the action scheduler.

Closes #

## Changes
- ...

## Type of Change
- [x] New feature

## Testing
`ctest --test-dir build` -> 152/152 passing.

## Checklist
- [x] Code compiles without errors
- [x] No new warnings introduced
- [x] Tests pass
- [x] Self-reviewed the diff
```

### R15 — Docstrings de test au format Given / When / Then

**Règle :** toute fonction de test (pytest `test_*`, et plus largement tout cas
de test du projet) doit porter une **docstring structurée en trois temps
`Given` / `When` / `Then`** : une clause par ligne — le contexte initial
(`Given`), l'action déclenchée (`When`), le résultat attendu (`Then`).

**Pourquoi :** le nom d'un test décrit *quoi*, pas *dans quelles conditions* ni
*ce qui est attendu*. Le triptyque Given/When/Then rend explicites le pré-état,
l'action et l'assertion : la docstring sert de mini-spec lisible, accélère la
revue et le diagnostic d'un échec, et pousse à ne valider qu'**un seul**
comportement par test.

**À appliquer :** tout nouveau test et tout test touché lors d'un refactoring.
Le format prime sur la longueur : une ligne par clause suffit. S'applique à tous
les langages de test du projet (pytest côté IA, GoogleTest côté C++ via un
commentaire `// Given / When / Then` au-dessus du corps si le framework n'a pas
de docstring).

**Exemple interdit :**

```python
def test_pop_line_single():
    """Test pop_line."""
    buf = Buffer()
    buf.feed("Forward\n")
    assert buf.pop_line() == "Forward"
```

**Exemple correct :**

```python
def test_pop_line_single():
    """
    Given a buffer fed with a single newline-terminated line
    When pop_line is called
    Then it returns the line without its newline
    """
    buf = Buffer()
    buf.feed("Forward\n")
    assert buf.pop_line() == "Forward"
```

### R16 — Un parseur parse, il ne valide pas le format d'entrée

**Règle :** une fonction ou un module de **parsing** ne fait qu'**une seule
chose** : transformer une entrée (chaîne, octets) en structure de données. Il
**suppose** que l'entrée respecte le format garanti par son producteur (ici le
serveur Zappy) et ne doit **ni vérifier** que l'entrée est « bien formée »,
**ni lever d'exception** sur une entrée inattendue, **ni embarquer** de
`try/except` ou de `if … : raise` de validation de format. Si une robustesse
sur des entrées non fiables est réellement nécessaire, elle vit dans une
**couche distincte en amont** du parseur, jamais dedans.

**Pourquoi :** séparation des responsabilités. Mêler parsing et validation
gonfle le parseur, multiplie des chemins d'erreur pénibles à tester, et duplique
une garantie déjà fournie par le protocole. Un parseur pur (entrée → sortie,
sans effet de bord ni branche d'erreur) est trivial à lire, à tester et à
réutiliser. Cette règle **précise** R13 : R13 dit *où* lèvent les erreurs
métier (exceptions custom dans `Exceptions/`) ; R16 dit qu'un parseur, lui, n'en
lève **pas** — ce n'est pas du code métier qui décide, c'est une transformation.

**À appliquer :** tout code de parsing (`ia/parsing/*`, parseurs de réponses
serveur, découpe de trames, etc.). Quand on est tenté d'ajouter une vérification
de format dans un parseur, c'est le signal que la vérification doit vivre
ailleurs — ou ne pas exister.

**Exemple interdit :**

```python
def parse_look(response, pos, direction, level):
    if not response.startswith("[") or not response.endswith("]"):
        raise ValueError("malformed Look response")   # INTERDIT : validation
    try:
        tiles = _split_tiles(response)
    except Exception:                                  # INTERDIT : garde-fou
        return []
    ...
```

**Exemple correct :**

```python
def parse_look(response, pos, direction, level):
    tiles = _split_tiles(response)
    return [
        {
            "coords": _tile_index_to_abs_coords(i, pos, direction, level),
            "objects": tiles[i],
        }
        for i in range(len(tiles))
    ]
```

### R17 — Docstrings Python : une ligne max, pas de paraphrase des types

**Règle :** dans le code Python **hors tests**, une docstring de
fonction / méthode / classe / module est **optionnelle** et, quand elle
existe, tient sur **une seule ligne** — un résumé de l'intention. Elle ne
doit **pas** contenir de sections `Args:` / `Returns:` / `Raises:` qui ne
font que répéter ce que la **signature typée** exprime déjà. Si une entrée
ou une sortie mérite une explication, ce sont le **type** (annotation) et
le **nom** (cf. R6) qui la portent ; au pire une demi-phrase dans le
résumé.

**Pourquoi :** les annotations de type documentent déjà les entrées et
sorties. Un bloc `Args:`/`Returns:` qui transforme `response: str` en
« response: the raw server line » est redondant, alourdit le fichier et
pourrit dès que la signature change — exactement le défaut que R10 combat
pour les commentaires de corps. Un résumé d'une ligne dit *pourquoi* la
fonction existe ; le *comment* est déjà dans le code et les types.

**À appliquer :** tout fichier `.py` du projet **sauf les tests** (qui
suivent R15 : docstring `Given`/`When`/`Then` multi-ligne). Vaut pour
toute nouvelle déclaration et toute déclaration touchée lors d'un
refactoring. pylint ne réclame pas de docstring
(`missing-*-docstring` désactivé) : l'**absence** est tolérée ; ce qui est
interdit, c'est la docstring multi-ligne à rallonge avec `Args:`/`Returns:`.

**Exemple interdit :**

```python
def _split_tiles(response: str) -> list[list[str]]:
    """Split a raw Look response into per-tile object lists.

    Args:
        response: Raw server line, e.g. "[player,,food]".
    Returns:
        One list of object names per tile, in server order.
    """
```

**Exemple correct :**

```python
def _split_tiles(response: str) -> list[list[str]]:
    """Split a raw Look response into one object list per tile."""
```
