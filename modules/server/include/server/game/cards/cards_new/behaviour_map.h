#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

// value is only unique ptr
template <typename Key, typename Value>
class StaticMap
{
    // check if its possible to implement this map as a base type, we can then use it in shared to store actual card
    // information and we can use it on server to store behaviours
    static std::unordered_map<Key, std::unique_ptr<Value>> map_;

public:
    StaticMap() = delete; // prevent instantiation

    /*
    the map should directly construct unique ptrs to the given type, so vargs for insert
    the map should also return references to things, so deref values

    would be cool to also be able to get a list of keys, so we could say:
    auto vector = map.get("card1", "card2", ...);

    functions with variadic arguments:
    insert(key, args...) // construct in place?
    get(key, keys...) // to either get one value or a list of values

    // no reason to get the whole map i assume?
    */

private:
    ~StaticMap() = default; // prevent instantiation
};


/*
maybe like this:

map<card_id, cardptr>
map<card_id, {behaviour_ids...(typeid)}>
map<behaviour_id, behaviour_ptr> ?
*/
