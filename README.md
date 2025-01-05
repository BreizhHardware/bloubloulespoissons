# bloubloulespoissons
Bloublou les poissons est un jeu de simulation de comportement de poisson en réseau ou bien en solo.

Le jeu est codé en C++ avec la librairie SDL2.

# 🌱 Branches en développement
- `main` : La branche principale du projet
- `WII-U` : La branche pour la version Wii-U du jeu

# 🚀 Installation et configuration
## Prérequis
- [SDL2](https://www.libsdl.org/download-2.0.php)
- [SDL2_image](https://www.libsdl.org/projects/SDL_image/)
- [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)
- [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/)
- [SDL2_net](https://www.libsdl.org/projects/SDL_net/)
- [CMake](https://cmake.org/download/)
- [Git](https://git-scm.com/downloads)
- [GCC](https://gcc.gnu.org/)
- [Make](https://www.gnu.org/software/make/)

## Compilation et lancement
1. Clonez le dépôt
```bash
git clone https://github.com/BreizhHardware/bloubloulespoissons.git
cd bloubloulespoissons
```
2. Créez un dossier build
```bash
mkdir build
cd build
```
3. Générez les fichiers de configuration
```bash
cmake ..
```
4. Compilez le projet
```bash
make -j
```
5. Lancez le jeu
```bash
./bloubloulespoissons
```

# To do
- [x] Voir les autres joueurs
- [x] Id Incrémental pour les players
- [x] Récupération des infos des champs de text pour le join
- [X] Affichage de l'ip dans le champs d'host (fonctionnalité retiré)
- [x] Requin adversaire
- [x] Choix des musiques pour le jeux
- [x] Pouvoir mute les musiques via la touche M
- [x] Refactorer le code pour le rendre plus lisible
