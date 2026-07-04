<div align="center">

<img src="docs/logo.png" alt="Nekron" width="96"/>

# N E K R O N

**A fast, bloody, pixel-art bullet heaven.**
One survivor. Infinite map. Endless waves of the dead.

![C++](https://img.shields.io/badge/C%2B%2B-23-blue?logo=cplusplus&logoColor=white)
![SFML](https://img.shields.io/badge/SFML-2.6.2-8CC445?logo=sfml&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.26%2B-064F8C?logo=cmake&logoColor=white)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)

<img src="docs/gameplay.png" alt="gameplay" width="820"/>

*Crits, minimap, muzzle flash, floating damage — mid-fight on wave 1.*

</div>

---

## ⚡ How long can you last?

The horde never stops. Every wave is bigger, faster and hungrier than the last.
Kite, dash, reload, throw a grenade into the crowd and carve your way to the next level-up.
Every 5th wave a **Boss** crawls out — and past wave 15 they come in pairs.

<div align="center">
<img src="docs/action.png" alt="shotgun action" width="410"/> <img src="docs/levelup.png" alt="level up" width="410"/>
</div>

## 🎮 Controls

| Key | Action |
|:---:|--------|
| `W A S D` | move |
| `mouse` | aim — weapon tracks the cursor |
| `LMB` | shoot (hold for full-auto AK) |
| `Space` | **dash** — short burst with invincibility frames |
| `G` | **grenade** — arcs to your cursor, big boom |
| `R` | reload |
| `scroll` / `1 2 3` / `Q` | switch weapons |
| `P` | pause |
| `Esc` | pause menu (resume / restart / music / quit) |
| `F11` | fullscreen |
| `H` `C` `M` | change hair / shirt / moustache |

## 🔫 Arsenal

| Weapon | Fire mode | Damage | Mag | Evolution *(after boss waves)* |
|--------|-----------|:------:|:---:|-------------------------------|
| **AK** | full-auto, 10 rps | 13 | 30 | 🔥 fires **2 bullets** per shot |
| **Shotgun** | per click | 7 × 11 | 6 | 🔥 **12 pellets** per blast |
| **Pistol** | semi-auto | 25 | 12 | 🔥 **piercing** rounds, +50% damage |
| **Grenade** | `G`, max 3 | 110 AoE | — | knockback, scorch marks, screen shake |

Crits hit for **double damage** (base 10% chance) with a white flash — stack crit upgrades and watch the yellow numbers fly.

## 🧟 Bestiary

| Enemy | HP | Trait |
|-------|:--:|-------|
| **Zombie** | 100+ | relentless walker |
| **Runner** *(wave 2+)* | 50+ | fast, bites hard |
| **Tank** *(wave 3+)* | 220+ | armored — shrugs off part of every hit |
| **Spitter** *(wave 4+)* | 70+ | keeps its distance and spits venom projectiles |
| **Boss** *(every 5th wave)* | 500+ | 💀 under 50% HP it **enrages**: faster, double damage |

All enemies gain HP, speed and damage every wave. There is no ceiling. You will die.

## 📈 Progression

- **XP** from every kill (and coins) fills the green bar → **level up** → pick 1 of 3 upgrades:
  damage, move speed, max HP, reload speed, fire rate, crit chance
- **Beat a boss wave** → choose a **weapon evolution**
- **Drops**: ammo, medkits, grenades, coins — magnet-pulled when you need them
- **Day/night cycle** — at night the street lamps matter
- **Best score** is saved between runs

## 🚀 Play it

**Easiest:** grab `Nekron-win64.zip` from [Releases](../../releases), unzip, double-click `Nekron.exe`. No install, no dependencies.

**Build from source:**

```sh
cmake -S . -B build-sfml -DUSE_SFML=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build-sfml -j 8
./build-sfml/Nekron
```

SFML 2.6.2 is fetched and built automatically — you only need CMake 3.26+ and a C++23 compiler.

<details>
<summary>🎓 Console demo build (no SFML, used by CI)</summary>

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j 8
./build/oop
```

Runs a console scenario exercising the game logic classes directly. Optional sanitizers via `-DUSE_ASAN=ON`, Valgrind via `./scripts/run_valgrind.sh`.
</details>

## 🔧 Under the hood

- **Model / view split** — game rules (`Game`, `Player`, `Zombie` hierarchy, `Weapon`, `EntityPool<T>`) know nothing about rendering; the SFML layer (`GameApp`, `WorldRenderer`, `Hud`, `CharacterSprite`) drives them
- **Infinite deterministic world** — the map streams in hashed chunks: same seed, same forest, no storage
- **Layered character** — body, shirt, hair, moustache and shadow are separate sprites composited per frame
- **100% procedural audio** — every gunshot, explosion and the music loop are synthesized at startup; drop your own `assets/music.ogg` to override the soundtrack
- **Zero-asset effects** — blood, decals, damage numbers, vignette, hit-stop, screen shake, day/night, minimap
- OOP toolbox: virtual hierarchies + `dynamic_cast`, factory & singleton patterns, custom exception hierarchy used for real control flow, templates, copy-and-swap, STL throughout

## 📜 License & credits

- Code licensed under [AGPLv3](LICENSE); based on the [oop-template](https://github.com/mcmarius/oop-template) ([Unlicense](LICENSE.template))
- Built with [SFML](https://www.sfml-dev.org/)
- Pixel-art sprite pack included in `assets/`; grenade sprite and all sound effects made for this project
