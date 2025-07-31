//talgov44@gmail.com

#pragma once

#include "Player.hpp"

namespace coup
{
    class Judge : public Player
    {
    public:
        Judge(Game &game, const std::string &name);
        ~Judge() override = default;

        // Judge's special undo ability
        void undo(Player &target_of_bribe) override;
    };
}