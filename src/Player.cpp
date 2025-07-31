//talgov44@gmail.com

#include "Player.hpp"
#include <iostream>

namespace coup
{

    const int BRIBE_COST = 4;
    const int SANCTION_COST = 3;
    const int COUP_COST = 7;
    const int MUST_COUP_COINS = 10;
    const int JUDGE_SANCTION_PENALTY = 1;
    const int MERCHANT_ARREST_PENALTY = 2;
    const int BARON_SANCTION_COMPENSATION = 1;

    Player::Player(Game &game, const std::string &name) : game(game),
                                                          _name(name),
                                                          _coins(0),
                                                          is_active(true),
                                                          _role("player"),
                                                          last_action(""),
                                                          _last_arrested_target(nullptr),
                                                          _is_sanctioned(false),
                                                          _has_extra_action(false),
                                                          _aggressor_in_last_coup(nullptr)
    {
        this->game.add_player(this);
    }

    /**
     * @brief Validates that it is currently this player's turn to act.
     *
     * Verifies two conditions: the player
     * must be active (not eliminated) and it must be their turn according to the
     * game state.
     *
     * @throws std::runtime_error If the player is not active.
     * @throws std::runtime_error If it is not the player's turn.
     */
    void Player::check_turn() const
    {
        if (!this->is_active)
        {
            throw std::runtime_error("Player " + this->_name + " is not active.");
        }
        if (this->game.turn() != this->_name)
        {
            throw std::runtime_error("It's not " + this->_name + "'s turn!");
        }
    }

    void Player::must_coup() const
    {
        if (this->_coins >= MUST_COUP_COINS)
        {
            throw std::runtime_error("Player must perform a coup with 10 or more coins.");
        }
    }

    void Player::end_turn_or_continue()
    {
        if (this->_has_extra_action)
        {
            this->_has_extra_action = false;
        }
        else
        {
            this->game.next_turn();
        }
    }

    void Player::cancelExtraAction()
    {
        this->_has_extra_action = false;
    }

    /**
     * @brief Performs the 'gather' action to gain 1 coin.
     *
     * This is a standard action available to all players that increases their coin
     * count by one. This action consumes the player's turn.
     *
     * If the player is sanctioned, this action will fail. The sanction will be
     * cleared, but the turn is still consumed, effectively wasting the turn.
     *
     * @throws std::runtime_error If it is not the player's turn.
     * @throws std::runtime_error If the player has 10 or more coins and is required to coup.
     * @throws std::runtime_error If the player is sanctioned for the turn.
     */
    void Player::gather()
    {
        check_turn();
        must_coup();
        if (this->_is_sanctioned)
        {
            this->_is_sanctioned = false;
            end_turn_or_continue(); // Turn or extra action is wasted
            throw std::runtime_error("Cannot use gather, you are sanctioned for this turn.");
        }
        this->_coins++;
        this->last_action = "gather";
        end_turn_or_continue();
    }

    /**
     * @brief Performs the 'tax' action to gain 2 coins.
     *
     * Increases the player's coin count by two.
     *
     * If the player is sanctioned, this action will fail. The sanction will be
     * cleared, but the turn is still consumed
     *
     * @throws std::runtime_error If it is not the player's turn.
     * @throws std::runtime_error If the player has 10 or more coins and is required to coup.
     * @throws std::runtime_error If the player is sanctioned for the turn.
     * @see Governor::tax()
     */
    void Player::tax()
    {
        check_turn();
        must_coup();
        if (this->_is_sanctioned)
        {
            this->_is_sanctioned = false;
            end_turn_or_continue();
            throw std::runtime_error("Cannot use tax, you are sanctioned for this turn.");
        }
        this->_coins += 2;
        this->last_action = "tax";
        end_turn_or_continue();
    }

    /**
     * @brief Performs the 'bribe' action to gain an extra action.
     *
     * This action costs 4 coins and grants the player an immediate second action
     * within the same turn. It does not end the player's turn.
     *
     * @throws std::runtime_error If it is not the player's turn.
     * @throws std::runtime_error If the player has 10 or more coins and is required to coup.
     * @throws std::runtime_error If the player has fewer than 4 coins.
     */
    void Player::bribe()
    {
        check_turn();
        must_coup();
        this->_is_sanctioned = false;
        if (this->_coins < BRIBE_COST)
        {
            throw std::runtime_error("Not enough coins for a bribe.");
        }
        this->removeCoins(BRIBE_COST);
        this->_has_extra_action = true;
        this->last_action = "bribe";
    }

    void Player::arrest(Player &target)
    {
        this->game.clearSaveWindow();
        check_turn();
        must_coup();
        this->_is_sanctioned = false;
        if (this == &target || !target.isActive())
        {
            throw std::runtime_error("Invalid target for arrest.");
        }
        if (&target == this->_last_arrested_target)
        {
            throw std::runtime_error("Cannot arrest the same player twice in a row.");
        }

        if (target.role() == "Merchant")
        {
            if (target.coins() < MERCHANT_ARREST_PENALTY)
            {
                throw std::runtime_error("Merchant target does not have enough coins to pay the penalty.");
            }
            target.removeCoins(MERCHANT_ARREST_PENALTY);
            // Arresting player gets nothing
        }
        else
        {

            if (target.coins() < 1)
            {
                throw std::runtime_error("Target has no coins to take.");
            }
            target.removeCoins(1);
            this->addCoins(1);
            if (target.role() == "General")
            {
                target.addCoins(1);
            }
        }

        this->_last_arrested_target = &target;
        this->last_action = "arrest";
        end_turn_or_continue();
    }

    /**
     * @brief Applies a one-turn sanction on a target player.
     *
     * This action costs 3 coins and consumes the player's turn. A sanctioned
     * player will have their next action (like gather or tax) fail, effectively
     * wasting their turn.
     *
     * @param target The player to apply the sanction to.
     * @throws std::runtime_error If it is not the player's turn.
     * @throws std::runtime_error If the player has 10 or more coins and must coup.
     * @throws std::runtime_error If the player cannot afford the sanction cost (including role-based penalties).
     * @throws std::runtime_error If the target is invalid (e.g., self or inactive).
     */
    void Player::sanction(Player &target)
    {
        check_turn();
        must_coup();
        this->_is_sanctioned = false;
        if (this->coins() < SANCTION_COST)
        {
            throw std::runtime_error("Not enough coins for a sanction.");
        }
        if (this == &target || !target.isActive())
        {
            throw std::runtime_error("Invalid target for sanction.");
        }
        this->removeCoins(SANCTION_COST);
        // If the target is a Judge, the sanctioner pays an extra coin.
        if (target.role() == "Judge")
        {
            this->removeCoins(JUDGE_SANCTION_PENALTY);
        }
        if (target.role() == "Baron")
        {
            target.addCoins(BARON_SANCTION_COMPENSATION);
        }

        target.setSanctioned(true);
        this->last_action = "sanction";
        end_turn_or_continue();
    }

    /**
     * @brief Performs the 'coup' action to eliminate another player from the game.
     *
     * This is a powerful action that costs 7 coins and permanently removes a target
     * player from the game. A coup is unstoppable by normal means, but some roles
     * may have a special ability to react to it.
     *
     * After a coup, a "save window" is opened, allowing a General to potentially
     * undo the elimination before the next player's turn.
     *
     * @param target The player to be eliminated.
     * @throws std::runtime_error If it is not the player's turn.
     * @throws std::runtime_error If the player has fewer than 7 coins.
     * @throws std::runtime_error If the player attempts to coup themselves.
     * @throws std::runtime_error If the target player is already inactive.
     */
    void Player::coup(Player &target)
    {
        check_turn();
        this->_is_sanctioned = false;
        if (this->_coins < COUP_COST)
        {
            throw std::runtime_error("Not enough coins for a coup (needs 7)!");
        }
        if (this == &target)
        {
            throw std::runtime_error("Cannot perform a coup on yourself.");
        }
        if (!target.isActive())
        {
            throw std::runtime_error("Target player " + target.getName() + " is already out of the game.");
        }
        this->_coins -= COUP_COST;
        target.eliminate();
        this->game.setPlayerToSave(&target);
        this->last_action = "coup";
        end_turn_or_continue();
    }

    std::string Player::role() const { return this->_role; }
    int Player::coins() const { return this->_coins; }
    std::string Player::getName() const { return this->_name; }
    bool Player::isActive() const { return this->is_active; }
    std::string Player::getLastAction() const { return this->last_action; }
    Player *Player::getLastArrestedTarget() const { return this->_last_arrested_target; }
    Player *Player::getAggressorInLastCoup() const { return this->_aggressor_in_last_coup; }
    void Player::addCoins(int amount) { this->_coins += amount; }
    void Player::removeCoins(int amount) { this->_coins = std::max(0, this->_coins - amount); }
    void Player::eliminate() { this->is_active = false; }
    void Player::revive()
    {
        this->is_active = true;
        this->_aggressor_in_last_coup = nullptr;
    }
    void Player::setSanctioned(bool status) { this->_is_sanctioned = status; }

    void Player::undo(Player &target)
    {
        (void)target;
        throw std::runtime_error("This player (" + this->role() + ") cannot undo actions.");
    }

}