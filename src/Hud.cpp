#include "../include/Hud.h"
#include "../include/TextureManager.h"
#include "../include/SpriteRects.h"
#include <algorithm>

namespace {
const char* kFontCandidates[] = {
    "C:/Windows/Fonts/consolab.ttf",
    "C:/Windows/Fonts/consola.ttf",
    "C:/Windows/Fonts/arialbd.ttf",
    "C:/Windows/Fonts/arial.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
    "/System/Library/Fonts/Supplemental/Arial Bold.ttf",
    "/System/Library/Fonts/Supplemental/Arial.ttf",
};

constexpr float PANEL_X = 14.f;
constexpr float PANEL_Y = 14.f;
constexpr float PANEL_W = 520.f;
constexpr float PANEL_H = 158.f;
constexpr float BAR_X = PANEL_X + 26.f;
}

Hud::Hud() {
    auto& tm = TextureManager::instance();

    barBgBig.setTexture(tm.get("ui"));
    barBgBig.setTextureRect(sheet::UI_BAR_BG_BIG);
    barRed.setTexture(tm.get("ui"));
    barBgSmall.setTexture(tm.get("ui"));
    barBgSmall.setTextureRect(sheet::UI_BAR_BG_SMALL);
    barGreen.setTexture(tm.get("ui"));
    ammoPip.setTexture(tm.get("ui"));
    ammoPip.setTextureRect(sheet::UI_AMMO_PIP);
    weaponIcon.setTexture(tm.get("weapons"));

    grenadePip.setTexture(tm.get("grenade"));
    grenadePip.setScale(1.6f, 1.6f);

    for (const char* path : kFontCandidates)
        if (font.loadFromFile(path)) {
            hasFont = true;
            break;
        }
}

void Hud::drawText(sf::RenderTarget& target, const std::string& str, float x, float y,
                   unsigned int size, sf::Color color, bool centered) {
    if (!hasFont)
        return;
    sf::Text text(str, font, size);
    text.setFillColor(color);
    text.setOutlineColor(sf::Color(20, 20, 25, color.a));
    text.setOutlineThickness(2.f);
    if (centered) {
        const sf::FloatRect b = text.getLocalBounds();
        text.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    }
    text.setPosition(x, y);
    target.draw(text);
}

void Hud::draw(sf::RenderWindow& window, const HudState& state, const sf::View& uiView) {
    const sf::View previous = window.getView();
    window.setView(uiView);
    const sf::Vector2f screen(uiView.getSize());

    if (state.menu) {
        sf::RectangleShape overlay(screen);
        overlay.setFillColor(sf::Color(10, 12, 8, 150));
        window.draw(overlay);
        drawText(window, "NEKRON", screen.x / 2.f, screen.y * 0.30f, 110,
                 sf::Color(200, 40, 40), true);
        drawText(window, "Enter = start", screen.x / 2.f, screen.y * 0.50f, 30,
                 sf::Color::White, true);
        drawText(window, "Best score: " + std::to_string(state.bestScore),
                 screen.x / 2.f, screen.y * 0.58f, 24, sf::Color(255, 220, 120), true);
        drawText(window, "WASD move | Space dash | LMB shoot | G grenade | R reload | scroll weapons | F11 fullscreen",
                 screen.x / 2.f, screen.y - 24.f, 17, sf::Color(235, 235, 235, 200), true);
        window.setView(previous);
        return;
    }

    const float hpW = 272.f;
    const float hpH = 46.f;
    barBgBig.setScale(hpW / static_cast<float>(sheet::UI_BAR_BG_BIG.width),
                      hpH / static_cast<float>(sheet::UI_BAR_BG_BIG.height));
    barBgBig.setPosition(BAR_X, PANEL_Y + 18.f);
    window.draw(barBgBig);
    const float hpRatio = std::clamp(static_cast<float>(state.hp) / static_cast<float>(state.maxHp), 0.f, 1.f);
    const int hpWidth = static_cast<int>(static_cast<float>(sheet::UI_BAR_RED.width) * hpRatio);
    if (hpWidth > 0) {
        barRed.setTextureRect({sheet::UI_BAR_RED.left, sheet::UI_BAR_RED.top,
                               hpWidth, sheet::UI_BAR_RED.height});
        barRed.setScale((hpW - 10.f) / static_cast<float>(sheet::UI_BAR_RED.width),
                        (hpH - 10.f) / static_cast<float>(sheet::UI_BAR_RED.height));
        barRed.setPosition(BAR_X + 5.f, PANEL_Y + 23.f);
        window.draw(barRed);
    }
    drawText(window, std::to_string(state.hp) + " / " + std::to_string(state.maxHp),
             BAR_X + 10.f, PANEL_Y + 26.f, 22, sf::Color::White);

    const float xpW = 272.f;
    const float xpH = 28.f;
    barBgSmall.setScale(xpW / static_cast<float>(sheet::UI_BAR_BG_SMALL.width),
                        xpH / static_cast<float>(sheet::UI_BAR_BG_SMALL.height));
    barBgSmall.setPosition(BAR_X, PANEL_Y + 80.f);
    window.draw(barBgSmall);
    const int xpWidth = static_cast<int>(static_cast<float>(sheet::UI_BAR_GREEN.width)
                                         * std::clamp(state.waveProgress, 0.f, 1.f));
    if (xpWidth > 0) {
        barGreen.setTextureRect({sheet::UI_BAR_GREEN.left, sheet::UI_BAR_GREEN.top,
                                 xpWidth, sheet::UI_BAR_GREEN.height});
        barGreen.setScale((xpW - 8.f) / static_cast<float>(sheet::UI_BAR_GREEN.width),
                          (xpH - 8.f) / static_cast<float>(sheet::UI_BAR_GREEN.height));
        barGreen.setPosition(BAR_X + 4.f, PANEL_Y + 84.f);
        window.draw(barGreen);
    }

    const float pipScale = state.maxAmmo > 16 ? 1.7f : 2.6f;
    const float pipStep = 8.f * pipScale + 3.f;
    ammoPip.setColor(sf::Color(255, 255, 255, state.reloading ? 80 : 255));
    for (int i = 0; i < state.ammo; i++) {
        ammoPip.setScale(pipScale, pipScale);
        ammoPip.setPosition(BAR_X + static_cast<float>(i) * pipStep, PANEL_Y + 122.f);
        window.draw(ammoPip);
    }
    ammoPip.setColor(sf::Color::White);

    for (int i = 0; i < state.grenades; i++) {
        grenadePip.setPosition(BAR_X + static_cast<float>(i) * 26.f, PANEL_Y + 146.f);
        window.draw(grenadePip);
    }

    const float iconX = PANEL_X + PANEL_W - 130.f;
    weaponIcon.setTextureRect(state.weaponIcon);
    weaponIcon.setScale(3.6f, 3.6f);
    weaponIcon.setColor(state.weaponEvolved ? sf::Color(255, 215, 130) : sf::Color::White);
    weaponIcon.setPosition(iconX,
                           PANEL_Y + 62.f - static_cast<float>(state.weaponIcon.height) * 3.6f / 2.f);
    window.draw(weaponIcon);
    if (state.reloading) {
        ammoPip.setScale(12.f * std::clamp(state.reloadProgress, 0.f, 1.f), 1.6f);
        ammoPip.setPosition(iconX, PANEL_Y + 92.f);
        window.draw(ammoPip);
    }

    drawText(window, "Wave " + std::to_string(state.wave)
                     + "   Score " + std::to_string(state.score),
             PANEL_X + 8.f, PANEL_Y + PANEL_H + 28.f, 24, sf::Color::White);

    if (!state.banner.empty() && state.bannerAlpha > 0.f) {
        const auto a = static_cast<sf::Uint8>(255.f * std::clamp(state.bannerAlpha, 0.f, 1.f));
        drawText(window, state.banner, screen.x / 2.f, screen.y * 0.30f, 64,
                 sf::Color(255, 235, 160, a), true);
    }

    if (state.levelUp) {
        sf::RectangleShape overlay(screen);
        overlay.setFillColor(sf::Color(0, 0, 0, 140));
        window.draw(overlay);
        drawText(window, "LEVEL UP!", screen.x / 2.f, screen.y * 0.30f, 60,
                 sf::Color(150, 255, 130), true);
        for (std::size_t i = 0; i < state.upgrades.size(); i++)
            drawText(window, std::to_string(i + 1) + ")  " + state.upgrades[i],
                     screen.x / 2.f, screen.y * 0.44f + static_cast<float>(i) * 52.f,
                     30, sf::Color::White, true);
        drawText(window, "press 1 / 2 / 3", screen.x / 2.f, screen.y * 0.70f, 22,
                 sf::Color(200, 200, 200), true);
    }

    if (state.pauseMenu) {
        sf::RectangleShape overlay(screen);
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);
        drawText(window, "PAUSED", screen.x / 2.f, screen.y * 0.24f, 56,
                 sf::Color(235, 235, 235), true);
        const std::string options[5] = {
            "Resume",
            "Restart run",
            std::string("Music: ") + (state.musicOn ? "ON" : "OFF"),
            "Main menu",
            "Quit game",
        };
        for (int i = 0; i < 5; i++) {
            const bool selected = i == state.menuIndex;
            drawText(window, (selected ? "> " : "") + options[i] + (selected ? " <" : ""),
                     screen.x / 2.f, screen.y * 0.38f + static_cast<float>(i) * 48.f,
                     selected ? 32 : 26,
                     selected ? sf::Color(255, 220, 110) : sf::Color(210, 210, 210), true);
        }
        drawText(window, "W/S select | Enter confirm | Esc resume",
                 screen.x / 2.f, screen.y * 0.78f, 20, sf::Color(180, 180, 180), true);
    }

    if (state.paused && !state.gameOver) {
        sf::RectangleShape overlay(screen);
        overlay.setFillColor(sf::Color(0, 0, 0, 110));
        window.draw(overlay);
        drawText(window, "PAUSED", screen.x / 2.f, screen.y * 0.42f, 64,
                 sf::Color(235, 235, 235), true);
        drawText(window, "P = resume", screen.x / 2.f, screen.y * 0.52f, 24,
                 sf::Color(200, 200, 200), true);
    }

    if (state.gameOver) {
        sf::RectangleShape overlay(screen);
        overlay.setFillColor(sf::Color(70, 0, 0, 160));
        window.draw(overlay);
        drawText(window, "GAME OVER", screen.x / 2.f, screen.y * 0.40f, 84,
                 sf::Color(255, 90, 70), true);
        drawText(window, "Score " + std::to_string(state.score)
                         + "  |  Kills " + std::to_string(state.kills)
                         + "  |  Best " + std::to_string(state.bestScore)
                         + "  |  Enter = restart",
                 screen.x / 2.f, screen.y * 0.52f, 28, sf::Color::White, true);
    }

    window.setView(previous);
}

const sf::Font* Hud::fontPtr() const {
    return hasFont ? &font : nullptr;
}
