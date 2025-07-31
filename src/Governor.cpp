//talgov44@gmail.com

#include "Governor.hpp"
#include <iostream>

namespace coup
{
    Governor::Governor(Game &game, const std::string &name) : Player(game, name)
    {

        this->_role = "Governor";
    }

    // A Governor takes 3 coins when using tax.
    void Governor::tax()
    {
        check_turn();
        must_coup();
        if (this->_is_sanctioned)
        {
            this->_is_sanctioned = false;
            end_turn_or_continue();
            throw std::runtime_error("Cannot use tax, you are sanctioned for this turn.");
        }
        this->addCoins(3);
        this->last_action = "tax";
        end_turn_or_continue();
    }

    // A Governor can undo another player's tax action.
    // This action does not cost a turn.
    void Governor::undo(Player &target)
    {
        if (!target.isActive() || this == &target)
        {
            throw std::runtime_error("Invalid undo target.");
        }
        if (target.getLastAction() != "tax")
        {
            throw std::runtime_error("Governor can only undo a 'tax' action.");
        }
        // A normal tax gives 2 coins.
        target.removeCoins(2);
    }
}