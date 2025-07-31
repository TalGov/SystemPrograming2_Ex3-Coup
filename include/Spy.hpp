//talgov44@gmail.com

#include "Player.hpp"
#include <iostream>

namespace coup
{
    class Spy : public Player
    {
    public:
        Spy(Game &game, const std::string &name);
        ~Spy() override = default;

        // Spy's special undo ability
        void undo(Player &arresting_player) override;

        // Spy's unique ability to see coins. This is a non-turn action.
        void spyOn(const Player &target) const;
    };
}