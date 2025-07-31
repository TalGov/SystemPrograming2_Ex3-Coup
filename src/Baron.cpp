//talgov44@gmail.com

#include "Baron.hpp"

namespace coup
{
    const int INVEST_COST = 3;
    const int INVEST_RETURN = 6;

    Baron::Baron(Game &game, const std::string &name) : Player(game, name)
    {
        this->_role = "Baron";
    }

    /**
     * @brief A Baron can invest 3 coins to receive 6 in return.
     * This is a standard turn action.
     */
    void Baron::invest()
    {
        this->game.clearSaveWindow();
        check_turn();
        must_coup();
        if (this->_is_sanctioned)
        {
            this->_is_sanctioned = false;
            end_turn_or_continue();
            throw std::runtime_error("Cannot use invest, you are sanctioned for this turn.");
        }
        if (this->coins() < INVEST_COST)
        {
            throw std::runtime_error("Not enough coins to invest.");
        }
        this->removeCoins(INVEST_COST);
        this->addCoins(INVEST_RETURN);
        this->last_action = "invest";
        end_turn_or_continue();
    }
}
