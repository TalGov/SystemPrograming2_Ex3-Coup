//talgov44@gmail.com

#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Player.hpp"

namespace coup
{
    class Player;

    class Game
    {
    private:
        std::vector<Player *> _players;
        size_t _turn_index;
        bool _game_started;
        std::string last_arrested_player;
        Player *_player_to_be_saved;

    public:
        Game();
        ~Game();

        std::string turn();
        std::vector<std::string> players();
        std::string winner();

        void add_player(Player *player);
        void next_turn();
        size_t active_players_count() const;
        const std::vector<Player *> &get_players() const;

        void setPlayerToSave(Player *player);
        Player *getPlayerToSave() const;
        void clearSaveWindow();
    };
}
