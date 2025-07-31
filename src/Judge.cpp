//talgov44@gmail.com

#include "Judge.hpp"
#include <iostream>

#include "Game.hpp"

namespace coup
{

    Judge::Judge(Game &game, const std::string &name) : Player(game, name)
    {
        this->_role = "Judge";
    }

    /**
     * @brief Cancels a 'bribe' action performed by another player.
     *
     * @param target_of_bribe The player who just performed the 'bribe' action.
     * @throws std::runtime_error If the target player's last action was not 'bribe'.
     */
    void Judge::undo(Player &target_of_bribe)
    {
        if (target_of_bribe.getLastAction() != "bribe")
        {
            throw std::runtime_error("Judge can only undo a 'bribe' action.");
        }
        target_of_bribe.cancelExtraAction();
    }
}