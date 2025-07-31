//talgov44@gmail.com

#pragma once

#include "Player.hpp"

namespace coup
{
    class Baron : public Player
    {
    public:
        Baron(Game &game, const std::string &name);
        ~Baron() override = default;

        // Baron's special action
        void invest();
    };
}