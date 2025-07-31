//talgov44@gmail.com

#include "Spy.hpp"

namespace coup
{
    const int MERCHANT_ARREST_PENALTY = 2;

    Spy::Spy(Game &game, const std::string &name) : Player(game, name)
    {
        this->_role = "Spy";
    }

    /**
     * @brief A Spy can undo an 'arrest' action performed by another player.
     * This reverses the coin transfer, accounting for special roles.
     * @param arresting_player The player who performed the arrest.
     */
    void Spy::undo(Player &arresting_player)
    {
        if (arresting_player.getLastAction() != "arrest")
        {
            throw std::runtime_error("Spy can only undo an 'arrest' action.");
        }

        Player *arrested_player = arresting_player.getLastArrestedTarget();
        if (arrested_player == nullptr)
        {
            throw std::runtime_error("No recent arrest to undo.");
        }

        // Handle the reversal based on the role of the player who was arrested.
        if (arrested_player->role() == "Merchant")
        {
            // The Merchant paid a 2-coin penalty directly to the bank.
            // The undo action gives those 2 coins back to the Merchant.
            arrested_player->addCoins(MERCHANT_ARREST_PENALTY);
        }
        else
        {
            // For other players, the arresting player took 1 coin.
            // The undo action reverses this transfer.
            arresting_player.removeCoins(1);
            arrested_player->addCoins(1);
        }
    }

    /**
     * @brief Allows the spy to see how many coins another player has.
     * This is purely for information and does not consume a turn.
     * @param target The player to spy on.
     */
    void Spy::spyOn(const Player &target) const
    {
        std::cout << "[SPY REPORT] " << this->getName() << " sees that "
                  << target.getName() << " has " << target.coins() << " coins." << std::endl;
    }
}