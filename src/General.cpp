//talgov44@gmail.com

#include "General.hpp"
#include <iostream>

namespace coup
{
    const int GENERAL_UNDO_COST = 5;

    General::General(Game &game, const std::string &name) : Player(game, name)
    {
        this->_role = "General";
    }

    /**
     * @brief Reverses a recently performed coup on another player.
     *
     * A General can use this ability to save a player who was just eliminated
     * by a coup. This action costs the General 5 coins and must be performed
     * immediately after the coup occurs, before any other player takes a turn.
     *
     * @param target_of_coup The player who was just eliminated by a coup and is
     *                       to be revived.
     * @throws std::runtime_error If the General has fewer than 5 coins.
     * @throws std::runtime_error If the target player is already active (i.e., was not eliminated).
     * @throws std::runtime_error If the target player was not the most recent victim of a coup,
     *                            or if the opportunity to undo has passed.
     */
    void General::undo(Player &target_of_coup)
    {
        if (this->coins() < GENERAL_UNDO_COST)
        {
            throw std::runtime_error("General needs 5 coins to undo a coup.");
        }
        // Can only undo a coup on a player who was actually eliminated.
        if (target_of_coup.isActive())
        {
            throw std::runtime_error("Cannot undo a coup on an active player.");
        }

        // Check if this player was the target of the most recent coup.
        // This opportunity is cleared once the next player takes an action.
        if (&target_of_coup != this->game.getPlayerToSave())
        {
            throw std::runtime_error("This player was not eliminated in the previous turn or the window to undo has closed.");
        }

        this->removeCoins(GENERAL_UNDO_COST);
        target_of_coup.revive();
        this->game.clearSaveWindow();
    }
}