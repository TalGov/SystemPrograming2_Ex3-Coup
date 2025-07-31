//talgov44@gmail.com

#pragma once

#include <string>
#include <stdexcept>
#include "Game.hpp"

namespace coup
{
    class Game;

    class Player
    {
    protected:
        Game &game;
        std::string _name;
        int _coins;
        bool is_active;
        std::string _role;
        std::string last_action;
        Player *_last_arrested_target;
        bool _is_sanctioned;
        bool _has_extra_action;
        Player *_aggressor_in_last_coup;

        void check_turn() const;
        void must_coup() const;
        void end_turn_or_continue();
        void revive();            // For General
        void cancelExtraAction(); // For Judge

        friend class General;
        friend class Judge;
        friend class Spy;

    public:
        Player *last_arrested = nullptr;

        Player(Game &game, const std::string &name);
        virtual ~Player() = default;

        // General Actions
        void gather();
        virtual void tax();
        virtual void coup(Player &target);
        void bribe();
        void arrest(Player &target);
        void sanction(Player &target);

        // Getters
        std::string role() const;
        int coins() const;
        std::string getName() const;
        bool isActive() const;
        std::string getLastAction() const;
        Player *getLastArrestedTarget() const;
        Player *getAggressorInLastCoup() const;

        // State modifiers
        void addCoins(int amount);
        void removeCoins(int amount);
        void eliminate();
        void setSanctioned(bool status);

        // Blocking actions
        virtual void undo(Player &target);
    };
}
