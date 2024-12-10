
#pragma once

namespace server
{
    /**
     * @brief The `VictoryCardBehaviour` class is an abstract base class for all
     * victory cards. It provides a common interface for all victory cards to
     * evaluate their victory points at the end of the game.
     */
    class VictoryCardBehaviour
    {
    public:
        virtual ~VictoryCardBehaviour() = default;

        /**
         * @brief Get the victory points that this card is worth.
         *
         * The function is provided with the complete deck of the player, so that
         * the card can evaluate its victory points based on the other cards in the
         * deck.
         *
         * @param deck The complete deck of the player.
         * @return The number of victory points that this card is worth.
         */
        virtual int getVictoryPoints(const std::vector<shared::CardBase::id_t> &deck) const = 0;
    };

    /**
     * @brief The `ConstantVictoryPoints` class is a concrete implementation of
     * the `VictoryCardBehaviour` class that provides a fixed number of victory
     * points for the card.
     *
     * @tparam N The number of victory points that this card is worth.
     */
    template <int N>
    class ConstantVictoryPoints : public VictoryCardBehaviour
    {
    public:
        int getVictoryPoints(const std::vector<shared::CardBase::id_t> & /*deck*/) const override { return N; }
    };

    /**
     * @brief The `VictoryPointsPerCard` class is a concrete implementation of
     * the `VictoryCardBehaviour` class that provides a fixed number of victory
     * points for each set of `perN` cards of a specific type in the deck.
     *
     * @tparam points The number of victory points that each set of `perN` cards
     * is worth.
     * @tparam perN The number of cards that are required to get the victory points.
     * @tparam Filter A callable object that takes a `shared::CardBase::id_t` and
     * returns `true` if the card is of the type that this card is looking for.
     */
    template <int points, int perN, auto Filter>
    class VictoryPointsPerNCards : public VictoryCardBehaviour
    {
    public:
        int getVictoryPoints(const std::vector<shared::CardBase::id_t> &deck) const override
        {
            int count = std::count_if(deck.begin(), deck.end(), Filter);
            return points * (count / perN);
        }
    };
} // namespace server
