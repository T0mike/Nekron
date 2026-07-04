#ifndef SPRITE_RECTS_H
#define SPRITE_RECTS_H
#include <SFML/Graphics/Rect.hpp>

namespace sheet {

inline constexpr float SCALE = 4.f;
inline constexpr float WEAPON_SCALE = 3.f;

inline constexpr int FRAME = 32;
inline constexpr int IDLE_FIRST = 0;
inline constexpr int IDLE_COUNT = 4;
inline constexpr int WALK_FIRST = 4;
inline constexpr int WALK_COUNT = 6;
inline constexpr float FEET_X = 16.f;
inline constexpr float FEET_Y = 28.f;
inline constexpr float HAND_OFFSET_Y = -7.f;

inline sf::IntRect frameRect(int index) {
    return {index * FRAME, 0, FRAME, FRAME};
}

inline const sf::IntRect RIFLE_RECT{0, 1, 28, 7};
inline const sf::IntRect SMG_RECT{0, 10, 28, 11};
inline const sf::IntRect PISTOL_RECT{0, 23, 14, 8};

inline const sf::IntRect UI_BAR_BG_BIG{7, 2, 47, 12};
inline const sf::IntRect UI_BAR_RED{8, 17, 47, 10};
inline const sf::IntRect UI_BAR_BG_SMALL{7, 32, 47, 8};
inline const sf::IntRect UI_BAR_GREEN{8, 42, 47, 6};
inline const sf::IntRect UI_AMMO_PIP{7, 50, 8, 5};

inline const sf::IntRect GRASS_TILE{6, 6, 36, 36};
inline const sf::IntRect UI_PANEL{48, 0, 48, 48};
inline const sf::IntRect DECOR_PEBBLES{113, 17, 9, 14};
inline const sf::IntRect DECOR_SMALL[6] = {
    {124, 18, 4, 3}, {124, 28, 4, 3}, {106, 34, 5, 3},
    {100, 41, 6, 3}, {122, 40, 4, 2}, {131, 42, 7, 4},
};

inline const sf::IntRect TREE_ROUND{3, 7, 34, 56};
inline const sf::IntRect TREE_PINE{39, 1, 26, 62};
inline const sf::IntRect DEAD_TREE{67, 35, 29, 28};
inline const sf::IntRect ROCK_BIG{80, 6, 21, 10};
inline const sf::IntRect ROCK_SMALL{107, 10, 12, 6};
inline const sf::IntRect BERRY_BUSH{98, 47, 26, 16};

inline const sf::IntRect SANDBAGS{14, 11, 48, 16};
inline const sf::IntRect CRATE{78, 27, 16, 16};
inline const sf::IntRect LAMP_POST{21, 43, 20, 80};
inline const sf::IntRect HOUSE{46, 47, 80, 76};
inline const sf::IntRect LIGHT_CONE_SOFT{165, 13, 68, 100};
inline const sf::IntRect LIGHT_CONE_HARD{292, 21, 58, 91};

}

#endif
