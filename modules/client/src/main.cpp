#include <iostream>
#include <iomanip>

#include "cards.h"

int main()
{
    auto print_nice = [ ] (auto a, auto b)
    {
        const size_t width = 20;
        std::cout << std::left << std::setw(width) << a
            << std::left << std::setw(width) << b
            << std::endl;
    };

    auto print_type = [&] (auto type)
    {
        for ( const auto& [card_id_str, card_ptr] : CardFactory::getCardMap() ) {
            if ( type == card_ptr->getCardType() ) {
                print_nice(card_id_str, card_ptr->getCardType());
            }
        }
    };

    std::cout << std::string(50, '-') << std::endl;
    print_nice("id", "type");
    std::cout << std::string(50, '-') << std::endl;
    print_type("Curse");
    std::cout << std::string(50, '-') << std::endl;
    print_type("Treasure");
    std::cout << std::string(50, '-') << std::endl;
    print_type("Action");
    std::cout << std::string(50, '-') << std::endl;
    print_type("Victory");
    std::cout << std::string(50, '-') << std::endl;
}
