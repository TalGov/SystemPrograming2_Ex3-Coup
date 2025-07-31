//talgov44@gmail.com

#pragma once

#include "Player.hpp"
#include "Game.hpp"

namespace coup
{
    class Governor : public Player
    {
    public:
        Governor(Game &game, const std::string &name);
        ~Governor() override = default;

        // Overridden actions
        void tax() override;

        // Undoing ability
        void undo(Player &target) override;
    };
}