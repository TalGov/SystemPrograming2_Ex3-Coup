//talgov44@gmail.com

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Game.hpp"
#include "Player.hpp"
#include "Governor.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include "Spy.hpp"
#include "Baron.hpp"

using namespace coup;
using namespace std;

struct Button
{
    sf::RectangleShape shape;
    sf::Text text;
    bool enabled = true;

    void setPosition(float x, float y)
    {
        shape.setPosition(x, y);
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
        text.setPosition(x + shape.getSize().x / 2.0f, y + shape.getSize().y / 2.0f);
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(shape);
        window.draw(text);
    }

    bool isClicked(const sf::Vector2f &mousePos) const
    {
        return enabled && shape.getGlobalBounds().contains(mousePos);
    }
};

// GUI element for a clickable player panel
struct PlayerPanel
{
    sf::RectangleShape shape;
    sf::Text text;
    Player *player_ref;

    bool isClicked(const sf::Vector2f &mousePos) const
    {
        return shape.getGlobalBounds().contains(mousePos);
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(shape);
        window.draw(text);
    }
};

int main()
{
    // --- Game Setup ---
    Game game;
    Governor p1(game, "A: Governor");
    Baron p2(game, "B: Baron");
    Spy p3(game, "C: Spy");
    Merchant p4(game, "D: Merchant");

    map<string, Player *> player_map;
    player_map[p1.getName()] = &p1;
    player_map[p2.getName()] = &p2;
    player_map[p3.getName()] = &p3;
    player_map[p4.getName()] = &p4;

    // --- SFML Setup ---
    sf::RenderWindow window(sf::VideoMode(800, 600), "Coup Game Demo");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
    {
        if (!font.loadFromFile("/usr/share/fonts/liberation/LiberationSans-Regular.ttf"))
        {
            cerr << "Error: Could not load font. Make sure a font file is available." << endl;
            return 1;
        }
    }

    // --- GUI State Variables ---
    sf::Text title("Coup Game", font, 24);
    title.setPosition(10, 10);
    sf::Text turn_text("", font, 20);
    turn_text.setPosition(10, 50);
    sf::Text message_text("", font, 16);
    message_text.setPosition(10, 550);
    message_text.setFillColor(sf::Color::Red);

    bool is_game_over = false;
    Player *selected_target = nullptr;

    // --- Action Buttons ---
    map<string, Button> buttons;
    vector<string> actions = {"gather", "tax", "coup", "invest", "arrest", "sanction", "bribe"};
    float button_y = 100;
    for (const auto &action : actions)
    {
        buttons[action].shape.setSize(sf::Vector2f(150, 40));
        buttons[action].text.setFont(font);
        buttons[action].text.setString(action);
        buttons[action].text.setCharacterSize(18);
        buttons[action].setPosition(620, button_y);
        button_y += 50;
    }

    // --- Player Panels ---
    vector<PlayerPanel> player_panels;
    float panel_y = 100;
    for (Player *p : game.get_players())
    {
        PlayerPanel panel;
        panel.player_ref = p;
        panel.shape.setSize(sf::Vector2f(580, 50));
        panel.shape.setPosition(20, panel_y);
        panel.text.setFont(font);
        panel.text.setCharacterSize(18);
        player_panels.push_back(panel);
        panel_y += 60;
    }
    // --- Get Stable Game State ONCE per Frame ---
    Player *current_player = nullptr;
    try
    {
        string current_turn_name = game.turn();
        current_player = player_map.at(current_turn_name);
        turn_text.setString("Turn: " + current_turn_name);
    }
    catch (const exception &e)
    {
        is_game_over = true;
        turn_text.setString("Game Over! Winner: " + game.winner());
    }

    // --- Main Game Loop ---
    while (window.isOpen())
    {

        // --- Event Handling ---
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (!is_game_over && event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {

                    sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

                    // Reset message on new action
                    message_text.setString("");

                    // Check for player panel clicks to select a target
                    for (const auto &panel : player_panels)
                    {
                        if (panel.isClicked(mousePos) && panel.player_ref != current_player && panel.player_ref->isActive())
                        {
                            selected_target = panel.player_ref;
                            break;
                        }
                    }

                    // Check for button clicks
                    try
                    {
                        if (buttons.at("gather").isClicked(mousePos))
                        {
                            current_player->gather();
                        }
                        else if (buttons.at("tax").isClicked(mousePos))
                        {
                            current_player->tax();
                        }
                        else if (buttons.at("bribe").isClicked(mousePos))
                        {
                            current_player->bribe();
                            message_text.setString("Bribe paid. Perform another action.");
                        }
                        else if (buttons.at("invest").isClicked(mousePos))
                        {
                            if (auto *baron = dynamic_cast<Baron *>(current_player))
                            {
                                baron->invest();
                            }
                        }
                        else if (selected_target)
                        { // Actions that require a target
                            if (buttons.at("coup").isClicked(mousePos))
                            {
                                current_player->coup(*selected_target);
                                selected_target = nullptr;
                            }
                            else if (buttons.at("arrest").isClicked(mousePos))
                            {
                                current_player->arrest(*selected_target);
                                selected_target = nullptr;
                            }
                            else if (buttons.at("sanction").isClicked(mousePos))
                            {
                                current_player->sanction(*selected_target);
                                selected_target = nullptr;
                            }
                        }
                    }
                    catch (const std::exception &e)
                    {
                        message_text.setString(e.what());
                    }

                    try
                    {
                        string current_turn_name = game.turn();
                        current_player = player_map.at(current_turn_name);
                        turn_text.setString("Turn: " + current_turn_name);
                    }
                    catch (const exception &e)
                    {
                        is_game_over = true;
                        turn_text.setString("Game Over! Winner: " + game.winner());
                    }
                }
            }
        }

        // --- Update GUI Elements ---
        // Disable all buttons by default
        for (auto &pair : buttons)
        {
            pair.second.enabled = false;
            pair.second.shape.setFillColor(sf::Color(100, 100, 100));
        }

        if (current_player && !is_game_over)
        {
            // Enable buttons based on game state
            buttons["gather"].enabled = current_player->coins() < 10;
            buttons["tax"].enabled = current_player->coins() < 10;
            buttons["bribe"].enabled = current_player->coins() >= 4 && current_player->coins() < 10;

            bool can_target = selected_target != nullptr;
            buttons["arrest"].enabled = can_target && current_player->coins() < 10;
            buttons["sanction"].enabled = can_target && current_player->coins() >= 3 && current_player->coins() < 10;
            buttons["coup"].enabled = can_target && current_player->coins() >= 7;

            if (auto *baron = dynamic_cast<Baron *>(current_player))
            {
                buttons["invest"].enabled = baron->coins() >= 3 && baron->coins() < 10;
            }

            // Recolor enabled buttons
            for (auto &pair : buttons)
            {
                if (pair.second.enabled)
                {
                    pair.second.shape.setFillColor(sf::Color(0, 150, 0));
                }
            }
        }

        // --- Drawing ---
        window.clear(sf::Color(20, 40, 60));
        window.draw(title);
        window.draw(turn_text);

        // Draw Player Panels
        for (auto &panel : player_panels)
        {
            Player *p = panel.player_ref;
            panel.text.setString(p->getName() + " (" + p->role() + ") | Coins: " + to_string(p->coins()));
            panel.text.setPosition(panel.shape.getPosition().x + 15, panel.shape.getPosition().y + 10);

            panel.shape.setOutlineThickness(2);
            panel.shape.setOutlineColor(sf::Color::White);

            if (!p->isActive())
            {
                panel.shape.setFillColor(sf::Color(80, 20, 20));
            }
            else if (p == current_player)
            {
                panel.shape.setFillColor(sf::Color(50, 80, 120));
            }
            else if (p == selected_target)
            {
                panel.shape.setFillColor(sf::Color(120, 110, 50));
            } // Highlight selected target
            else
            {
                panel.shape.setFillColor(sf::Color(50, 50, 50));
            }

            panel.draw(window);
        }

        for (auto &[key, val] : buttons)
        {
            val.draw(window);
        }
        window.draw(message_text);
        window.display();
    }

    return 0;
}
