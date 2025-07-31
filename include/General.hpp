//talgov44@gmail.com

#pragma once

#include "Player.hpp"
#include "Game.hpp"

namespace coup
{
    class General : public Player
    {
    public:
        General(Game &game, const std::string &name);
        ~General() override = default;

        void undo(Player &target_of_coup) override;
    };
}