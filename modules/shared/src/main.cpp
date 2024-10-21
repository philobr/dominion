/**
 * EXAMPLE IMPLEMENTATION USING TRAITS
 * IS MVP -> ROOM FOR IMPROVEMENT
 */

#include <iostream>
#include <vector>
#include <memory>
#include <tuple>
#include <string>
#include <typeinfo>

class Player {
    std::string name;
    int coins = 0;
    int actions = 0;
    int buys = 0;

public:
    Player(std::string name) : name(std::move(name)) { }

    void drawCard() { std::cout << name << " draws a card.\n"; }
    void gainActions(int n) { actions += n; std::cout << name << " gains " << n << " actions.\n"; }
    void gainCoins(int n) { coins += n; std::cout << name << " gains " << n << " coins.\n"; }
    void gainBuys(int n) { buys += n; std::cout << name << " gains " << n << " buys.\n"; }

    void discardDownTo(int n) { std::cout << name << " discards down to " << n << " cards.\n"; }
};

class Card {
public:
    virtual void play(Player& player, const std::vector<Player*>& otherPlayers) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getCategory() const = 0;
    virtual ~Card() = default;

    virtual std::string to_string() const
    {
        return "";
    }
};

// Specialized Card Types
class ActionCardBase : public Card {
    std::string name;
    std::string category;
public:
    ActionCardBase(std::string name, std::string category)
        : name(std::move(name)), category(std::move(category))
    { }

    std::string getName() const override { return name; }
    std::string getCategory() const override { return category; }

    virtual void play(Player& player, const std::vector<Player*>& otherPlayers) override = 0;
};

class TreasureCard : public Card {
    std::string name;
    std::string category;
    int value; // The number of coins this card gives

public:
    TreasureCard(int value, std::string name = "Treasure")
        : name(std::move(name)), category("Treasure Card"), value(value)
    { }

    void play(Player& player, const std::vector<Player*>&) override { player.gainCoins(value); }
    std::string getName() const override { return name; }
    std::string getCategory() const override { return category; }
};

class VictoryCard : public Card {
    std::string name;
    std::string category;
    int points; // The number of victory points

public:
    VictoryCard(int points, std::string name = "Victory")
        : name(std::move(name)), category("Victory Card"), points(points)
    { }

    void play(Player&, const std::vector<Player*>&) override
    {
        // Victory cards usually don't have play effects
    }

    std::string getName() const override
    {
        return name;
    }

    std::string getCategory() const override
    {
        return category;
    }
};

// Helper function to concatenate behavior strings
template <typename Tuple, std::size_t... I>
std::string concatBehaviors(const Tuple& behaviors, std::index_sequence<I...>)
{
    std::string result;
    ((result += std::get<I>(behaviors).to_string()), ...);
    return result;
}

// Modular Action Cards Using Traits
template <typename... Behaviors>
class ActionCard : public ActionCardBase {
    std::tuple<Behaviors...> behaviors;

public:
    ActionCard(std::string name, std::string category, Behaviors... behaviors)
        : ActionCardBase(std::move(name), std::move(category)), behaviors(std::make_tuple(behaviors...))
    { }

    ActionCard(std::string name, std::string category)
        : ActionCardBase(std::move(name), std::move(category)), behaviors(std::make_tuple(Behaviors()...))
    { }

    void play(Player& player, const std::vector<Player*>& otherPlayers) override
    {
        applyBehaviors(player, otherPlayers, std::index_sequence_for<Behaviors...>{});
    }

    std::string to_string() const override
    {
        std::string s = getName() + " (" + getCategory() + "):\n";
        s += concatBehaviors(behaviors, std::index_sequence_for<Behaviors...>{});
        return s;
    }

private:
    template <std::size_t... I>
    void applyBehaviors(Player& player, const std::vector<Player*>& otherPlayers, std::index_sequence<I...>)
    {
        (applyBehavior(std::get<I>(behaviors), player, otherPlayers), ...);
    }

    template <typename Behavior>
    void applyBehavior(Behavior& behavior, Player& player, const std::vector<Player*>& otherPlayers)
    {
        if constexpr ( std::is_invocable_v<decltype(&Behavior::apply), Behavior, Player&, const std::vector<Player*>&> ) {
            behavior.apply(player, otherPlayers);
        }
        else if constexpr ( std::is_invocable_v<decltype(&Behavior::apply), Behavior, Player&> ) {
            behavior.apply(player);
        }
        behaviour.apply(player)
    }
};

// Traits for Card Behaviors
template <int N>
struct DrawCards {
    void apply(Player& player) const
    {
        for ( int i = 0; i < N; ++i ) {
            player.drawCard();
        }
    }

    std::string to_string() const
    {
        return "Draw Cards: +" + std::to_string(N) + "\n";
    }
};

template <int N>
struct GainActions {
    void apply(Player& player) const
    {
        player.gainActions(N);
    }

    std::string to_string() const
    {
        return "Gain Actions: +" + std::to_string(N) + "\n";
    }
};

struct BaseAttribute {
    virtual std::string to_string() const = 0;
};

struct AttackAttribute : public BaseAttribute {
    virtual void apply(Player& player, const std::vector<Player*>& otherPlayers) const = 0;
    virtual std::string to_string() const = 0;
};

struct ActionAttribute : public BaseAttribute {
    virtual void apply(Player& player) const = 0;
    virtual std::string to_string() const = 0;
};

template <int N>
struct GainBuys {
    void apply(Player& player) const
    {
        player.gainBuys(N);
    }

    std::string to_string() const
    {
        return "Gain Buys: +" + std::to_string(N) + "\n";
    }
};

template <int N>
struct GainCoins {
    void apply(Player& player) const
    {
        player.gainCoins(N);
    }

    std::string to_string() const
    {
        return "Gain Coins: +" + std::to_string(N) + "\n";
    }
};

template <int N>
struct DiscardAttack {
    void apply(Player& player, const std::vector<Player*>& otherPlayers) const
    {
        for ( auto* other : otherPlayers ) {
            if ( other != &player ) {
                other->discardDownTo(N);
            }
        }
    }

    std::string to_string() const
    {
        return "Discard Attack: Down to " + std::to_string(N) + "\n";
    }
};

#define BUILD_CARD(name, type, ...) \
using name = ActionCard<__VA_ARGS__>; \
deck.push_back(std::make_unique<name>(#name, #type)); 

#define BUILD_ACTION_CARD(name, ...) \
BUILD_CARD(name, Action Card, __VA_ARGS__)


// Main Function to Demonstrate Usage
int main()
{
    Player alice("Alice");
    Player bob("Bob");
    std::vector<Player*> players = { &alice, &bob };

    std::vector<std::unique_ptr<Card>> deck;

    BUILD_ACTION_CARD(Laboratory, DrawCards<2>, GainActions<1>);
    BUILD_ACTION_CARD(Market, DrawCards<1>, GainActions<1>, GainBuys<1>, GainCoins<1>);
    BUILD_ACTION_CARD(Militia, DiscardAttack<3>);

    deck.push_back(std::make_unique<TreasureCard>(2, "Silver")); // Silver card
    deck.push_back(std::make_unique<VictoryCard>(3, "Duchy"));  // Duchy

    for ( auto& card : deck ) {
        std::cout << "Playing " << card->getName() << " (" << card->getCategory() << ")...\n";
        card->play(alice, players);

        std::cout << card->to_string() << std::endl;
    }

    return 0;
}
