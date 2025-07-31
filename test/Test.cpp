//talgov44@gmail.com

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.hpp"

#include "Game.hpp"
#include "Player.hpp"
#include "Governor.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include "Spy.hpp"
#include "Baron.hpp"

#include <vector>
#include <string>
#include <stdexcept>

using namespace coup;
using namespace std;

TEST_CASE("Game Setup and Basic State")
{
    Game game;
    SUBCASE("Cannot start game with less than 2 players")
    {
        Player p1(game, "Alice");
        CHECK_THROWS_AS(game.turn(), std::runtime_error);
        CHECK_THROWS_AS(game.winner(), std::runtime_error);
    }

    Player p1(game, "Alice");
    Player p2(game, "Bob");

    SUBCASE("Game starts correctly with 2 players")
    {
        CHECK_NOTHROW(game.turn());
        CHECK(game.turn() == "Alice");
        vector<string> players = game.players();
        CHECK(players.size() == 2);
        CHECK(players.at(0) == "Alice");
        CHECK(players.at(1) == "Bob");
    }

    SUBCASE("Cannot add players after game has started")
    {
        game.turn();
        CHECK_THROWS_AS(Player p3(game, "Charlie"), std::runtime_error);
    }

    SUBCASE("Players start with 0 coins")
    {
        CHECK(p1.coins() == 0);
        CHECK(p2.coins() == 0);
    }

    SUBCASE("Winner declaration logic")
    {
        CHECK_THROWS_AS(game.winner(), std::runtime_error);
        p1.addCoins(7);
        p1.coup(p2);
        CHECK_NOTHROW(game.winner());
        CHECK(game.winner() == "Alice");
        CHECK_THROWS_AS(p1.gather(), std::runtime_error);
    }
}

TEST_CASE("Core Player Actions and Turn Flow")
{
    Game game;
    Player p1(game, "Alice");
    Player p2(game, "Bob");
    Player p3(game, "Charlie");

    CHECK(game.turn() == "Alice");
    p1.gather();
    CHECK(p1.coins() == 1);
    CHECK(game.turn() == "Bob");

    p2.tax();
    CHECK(p2.coins() == 2);
    CHECK(game.turn() == "Charlie");

    CHECK_THROWS_AS(p1.gather(), std::runtime_error);
    p3.gather();
    CHECK(p3.coins() == 1);
    CHECK(game.turn() == "Alice");
}

TEST_CASE("Forced Coup at 10+ Coins")
{
    Game game;
    Player p1(game, "Alice");
    Player p2(game, "Bob");

    p1.addCoins(10);
    CHECK(p1.coins() == 10);

    SUBCASE("Must perform coup")
    {
        CHECK(game.turn() == "Alice");
        CHECK_THROWS_AS(p1.gather(), std::runtime_error);
        CHECK_THROWS_AS(p1.tax(), std::runtime_error);
        CHECK_THROWS_AS(p1.bribe(), std::runtime_error);
        CHECK_THROWS_AS(p1.arrest(p2), std::runtime_error);
        CHECK_THROWS_AS(p1.sanction(p2), std::runtime_error);
    }

    SUBCASE("Coup is allowed")
    {
        CHECK_NOTHROW(p1.coup(p2));
        CHECK(p1.coins() == 3);
        CHECK_FALSE(p2.isActive());
        CHECK(game.winner() == "Alice");
    }
}

TEST_CASE("Sanction Action and Effects")
{
    Game game;
    Player p1(game, "Sanker");
    Player p2(game, "Victim");
    Player p3(game, "ThirdParty");

    p1.addCoins(3);
    CHECK(game.turn() == "Sanker");
    CHECK_NOTHROW(p1.sanction(p2));
    CHECK(p1.coins() == 0);
    CHECK(game.turn() == "Victim");

    CHECK_THROWS_AS(p2.gather(), std::runtime_error);
    CHECK_THROWS_AS(p2.tax(), std::runtime_error);

    CHECK(game.turn() == "ThirdParty");

    p3.gather();
    p1.gather();

    CHECK(game.turn() == "Victim");
    CHECK_NOTHROW(p2.gather());
    CHECK(p2.coins() == 1);
}

TEST_CASE("Bribe Action")
{
    Game game;
    Player p1(game, "Briber");
    Player p2(game, "Bob");

    p1.addCoins(4);
    CHECK(game.turn() == "Briber");
    CHECK_NOTHROW(p1.bribe());
    CHECK(p1.coins() == 0);
    CHECK(game.turn() == "Briber");

    p1.gather();
    CHECK(p1.coins() == 1);
    CHECK(game.turn() == "Briber");
    p1.tax();
    CHECK(p1.coins() == 3);
    CHECK(game.turn() == "Bob");
}

TEST_CASE("Role: Governor")
{
    Game game;
    Governor gov(game, "Gov");
    Player p2(game, "TaxPayer");

    CHECK(game.turn() == "Gov");
    gov.tax();
    CHECK(gov.coins() == 3);
    CHECK(game.turn() == "TaxPayer");

    p2.tax();
    CHECK(p2.coins() == 2);
    CHECK(p2.getLastAction() == "tax");

    CHECK_NOTHROW(gov.undo(p2));
    CHECK(p2.coins() == 0);

    gov.gather();
    CHECK(game.turn() == "TaxPayer");
    p2.gather();
    CHECK_THROWS_AS(gov.undo(p2), std::runtime_error);
}

TEST_CASE("Role: General")
{
    Game game;
    General gen1(game, "Gen1");
    Player p2(game, "Arrester");
    General gen2(game, "Gen2");

    gen1.addCoins(1);
    CHECK(game.turn() == "Gen1");
    gen1.gather();
    CHECK(game.turn() == "Arrester");
    p2.arrest(gen1);
    CHECK(p2.coins() == 1);
    CHECK(gen1.coins() == 2);

    CHECK(game.turn() == "Gen2");
    gen2.addCoins(7);
    gen2.coup(p2);
    CHECK_FALSE(p2.isActive());

    CHECK(game.turn() == "Gen1");
    gen1.addCoins(5);
    CHECK_NOTHROW(gen1.undo(p2));
    CHECK(gen1.coins() == 2);
    CHECK(p2.isActive());
    CHECK(game.turn() == "Gen1");
}

TEST_CASE("Role: Judge")
{
    Game game;
    Judge judge(game, "Judy");
    Player p2(game, "Briber");
    Player p3(game, "Sanker");

    judge.gather();
    p2.addCoins(4);
    CHECK(game.turn() == "Briber");
    p2.bribe();
    CHECK(p2.getLastAction() == "bribe");

    judge.undo(p2);

    p2.gather();
    CHECK_THROWS_AS(p2.gather(), std::runtime_error);
    CHECK(game.turn() == "Sanker");

    p3.addCoins(4);
    CHECK(game.turn() == "Sanker");
    p3.sanction(judge);
    CHECK(p3.coins() == 0);
}

TEST_CASE("Role: Merchant")
{
    Game game;
    Merchant merch(game, "Manny");
    Player p2(game, "Arrester");

    merch.addCoins(3);
    game.turn();
    CHECK(merch.coins() == 4);
    merch.gather();
    CHECK(game.turn() == "Arrester");

    p2.arrest(merch);
    CHECK(p2.coins() == 0);
    CHECK(merch.coins() == 4);

    CHECK(game.turn() == "Manny");
    CHECK(merch.coins() == 4);
}

TEST_CASE("Role: Spy")
{
    Game game;
    Spy spy(game, "MataHari");
    Player p2(game, "Arrester");
    Player p3(game, "Victim");
    Merchant merch(game, "Richy");

    spy.gather();

    CHECK(game.turn() == "Arrester");
    p3.addCoins(1);
    p2.arrest(p3);
    CHECK(p2.coins() == 1);
    CHECK(p3.coins() == 0);
    spy.undo(p2);
    CHECK(p2.coins() == 0);
    CHECK(p3.coins() == 1);

    CHECK(game.turn() == "Victim");
    p3.gather();
    CHECK(game.turn() == "Richy");
    merch.tax();
    CHECK(game.turn() == "MataHari");
    spy.gather();

    CHECK(game.turn() == "Arrester");
    p2.arrest(merch);
    CHECK(p2.coins() == 0);
    CHECK(merch.coins() == 0);
    spy.undo(p2);
    CHECK(merch.coins() == 2);
}

TEST_CASE("Role: Baron")
{
    Game game;
    Baron baron(game, "Barry");
    Player p2(game, "Sanker");

    baron.addCoins(3);
    CHECK(game.turn() == "Barry");
    baron.invest();
    CHECK(baron.coins() == 6);

    p2.addCoins(3);
    CHECK(game.turn() == "Sanker");
    p2.sanction(baron);
    CHECK(p2.coins() == 0);
    CHECK(baron.coins() == 7);

    CHECK(game.turn() == "Barry");
    CHECK_THROWS_AS(baron.invest(), std::runtime_error);
}

TEST_CASE("Complex Interactions")
{
    SUBCASE("Bribe followed by a blocked action")
    {
        Game game;
        Player p1(game, "Briber");
        Governor gov(game, "Gov");

        p1.addCoins(4);
        p1.bribe();
        CHECK(game.turn() == "Briber");

        p1.tax();
        CHECK(p1.coins() == 2);

        gov.undo(p1);
        CHECK(p1.coins() == 0);

        CHECK(game.turn() == "Briber");
        CHECK_NOTHROW(p1.gather());
        CHECK(p1.coins() == 1);
        CHECK(game.turn() == "Gov");
    }
}
