//talgov44@gmail.com

#include "Merchant.hpp"

namespace coup
{
    Merchant::Merchant(Game &game, const std::string &name) : Player(game, name)
    {
        this->_role = "Merchant";
    }
}
