//talgov44@gmail.com

#include "Game.hpp"
#include "Player.hpp"
#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace coup
{
    const int MERCHANT_BONUS_THRESHOLD = 3;

    Game::Game() : _turn_index(0), _game_started(false) {}

    Game::~Game() {}

    /**
     * @brief Adds a new player to the game.
     *
     * This function adds a player to the game's player list. It can only be called
     * before the game has started. The game is considered started once the first turn
     * is taken.
     *
     * @param player A pointer to the Player object to be added. The Game does not
     *               take ownership of the pointer.
     * @throws std::runtime_error if the game has already started.
     * @throws std::runtime_error if the game is full (maximum of 6 players).
     */
    void Game::add_player(Player *player)
    {
        const size_t max_players = 6;
        const size_t min_players_to_start = 2;
        if (_game_started && this->players().size() >= min_players_to_start)
        {
            throw std::runtime_error("Game has already started, cannot add more players.");
        }
        if (_players.size() >= max_players)
        {
            throw std::runtime_error("Game is full, cannot add more players.");
        }
        _players.push_back(player);
    }

    void Game::setPlayerToSave(Player *player)
    {
        this->_player_to_be_saved = player;
    }

    Player *Game::getPlayerToSave() const
    {
        return this->_player_to_be_saved;
    }

    void Game::clearSaveWindow()
    {
        this->_player_to_be_saved = nullptr;
    }

    const std::vector<Player *> &Game::get_players() const
    {
        return _players;
    }

    /**
     * @brief Determines and returns the name of the current player whose turn it is.
     *
     * This function is responsible for identifying the active player for the current turn.
     * It handles the initial game start, ensures there are enough players to proceed,
     * and automatically skips any players who have been eliminated from the game.
     *
     * @return std::string The name of the player whose turn it is.
     * @throws std::runtime_error If there are no players in the game.
     * @throws std::runtime_error If the game has ended (fewer than 2 active players and game already started).
     * @throws std::runtime_error If the game has not started and there are fewer than 2 players.
     *
     * @note This function also prints the current player's name to standard output.
     * @note The game is considered started after the first successful call to this function.
     */
    std::string Game::turn()
    {
        if (_players.empty())
        {
            throw std::runtime_error("No players in the game.");
        }
        if (active_players_count() < 2 && _game_started)
        {
            throw std::runtime_error("Game has ended.");
        }
        if (!_game_started && _players.size() < 2)
        {
            throw std::runtime_error("Need at least 2 players to start the game.");
        }

        while (!_players.at(_turn_index)->isActive())
        {
            _turn_index = (_turn_index + 1) % _players.size();
        }

        // Apply start-of-turn effects before returning the player's name
        if (!_game_started)
        {
            Player *currentPlayer = _players.at(_turn_index);
            if (currentPlayer->role() == "Merchant" && currentPlayer->coins() >= MERCHANT_BONUS_THRESHOLD)
            {
                currentPlayer->addCoins(1);
            }
        }

        _game_started = true;
        std::cout << _players.at(_turn_index)->getName() << std::endl;
        return _players.at(_turn_index)->getName();
    }

    std::vector<std::string> Game::players()
    {
        std::vector<std::string> active_players_names;
        for (const auto &p : _players)
        {
            if (p->isActive())
            {
                active_players_names.push_back(p->getName());
            }
        }
        return active_players_names;
    }

    std::string Game::winner()
    {
        if (!_game_started)
        {
            throw std::runtime_error("Game has not started yet.");
        }
        if (active_players_count() != 1)
        {
            throw std::runtime_error("Game is still active or has not concluded.");
        }

        for (const auto &p : _players)
        {
            if (p->isActive())
            {
                return p->getName();
            }
        }
        throw std::runtime_error("Could not determine winner.");
    }

    /**
     * @brief Advances the game to the next active player's turn.
     *
     * This function is called at the end of a player's action to progress the game.
     * It finds the next player in the turn order who has not been eliminated.
     *
     * It is also responsible for applying any "start-of-turn" effects for the
     * upcoming player.
     */
    void Game::next_turn()
    {
        if (!_game_started)
        {
            _game_started = true;
        }
        if (active_players_count() < 2)
        {
            return; // Game is over
        }

        do
        {
            _turn_index = (_turn_index + 1) % _players.size();
        } while (!_players.at(_turn_index)->isActive());

        // Apply start-of-turn effects for the next player
        Player *nextPlayer = _players.at(_turn_index);
        if (nextPlayer->role() == "Merchant" && nextPlayer->coins() >= MERCHANT_BONUS_THRESHOLD)
        {
            nextPlayer->addCoins(1);
        }
    }

    size_t Game::active_players_count() const
    {
        size_t count = 0;
        for (const auto &p : _players)
        {
            if (p->isActive())
            {
                count++;
            }
        }
        return count;
    }

}