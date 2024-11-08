
#pragma once


// ======= GETTER MACROS ======= //


#define GET_STRING_MEMBER(var, document, member)                                                                       \
    if ( !document.HasMember(member) || !document[member].IsString() ) {                                               \
        return nullptr;                                                                                                \
    }                                                                                                                  \
    var = document[member].GetString();

#define GET_UINT_MEMBER(var, document, member)                                                                         \
    if ( !document.HasMember(member) || !document[member].IsUint() ) {                                                 \
        return nullptr;                                                                                                \
    }                                                                                                                  \
    var = document[member].GetUint();

#define GET_BOOL_MEMBER(var, document, member)                                                                         \
    if ( !document.HasMember(member) || !document[member].IsBool() ) {                                                 \
        return nullptr;                                                                                                \
    }                                                                                                                  \
    var = document[member].GetBool();

#define GET_STRING_ARRAY_MEMBER(var, document, member)                                                                 \
    var = std::vector<std::string>();                                                                                  \
    if ( !document.HasMember(member) || !document[member].IsArray() ) {                                                \
        return nullptr;                                                                                                \
    }                                                                                                                  \
    for ( const auto &elem : document[member].GetArray() ) {                                                           \
        if ( !elem.IsString() ) {                                                                                      \
            return nullptr;                                                                                            \
        }                                                                                                              \
        var.push_back(elem.GetString());                                                                               \
    }

#define GET_UINT_ARRAY_MEMBER(var, document, member)                                                                   \
    var = std::vector<unsigned int>();                                                                                 \
    if ( !document.HasMember(member) || !document[member].IsArray() ) {                                                \
        return nullptr;                                                                                                \
    }                                                                                                                  \
    for ( const auto &elem : document[member].GetArray() ) {                                                           \
        if ( !elem.IsUint() ) {                                                                                        \
            return nullptr;                                                                                            \
        }                                                                                                              \
        var.push_back(elem.GetUint());                                                                                 \
    }

#define GET_OPTIONAL_STRING_MEMBER(var, document, member)                                                              \
    if ( document.HasMember(member) ) {                                                                                \
        if ( !document[member].IsString() ) {                                                                          \
            return nullptr;                                                                                            \
        }                                                                                                              \
        var = document[member].GetString();                                                                            \
    } else {                                                                                                           \
        var = std::nullopt;                                                                                            \
    }


// ======= SETTER MACROS ======= //


#define ADD_STRING_MEMBER(var, key)                                                                                    \
    Value key##_value;                                                                                                 \
    key##_value.SetString(var, doc.GetAllocator());                                                                    \
    doc.AddMember(#key, key##_value, doc.GetAllocator());

#define ADD_UINT_MEMBER(var, key)                                                                                      \
    Value key##_value;                                                                                                 \
    key##_value.SetUint(var);                                                                                          \
    doc.AddMember(#key, key##_value, doc.GetAllocator());

#define ADD_OPTIONAL_STRING_MEMBER(var, key)                                                                           \
    if ( var ) {                                                                                                       \
        Value key##_value;                                                                                             \
        key##_value.SetString(var.value().c_str(), doc.GetAllocator());                                                \
        doc.AddMember(#key, key##_value, doc.GetAllocator());                                                          \
    }

#define ADD_BOOL_MEMBER(var, key)                                                                                      \
    Value key##_value;                                                                                                 \
    key##_value.SetBool(var);                                                                                          \
    doc.AddMember(#key, key##_value, doc.GetAllocator());

#define ADD_ARRAY_OF_STRINGS_MEMBER(var, key)                                                                          \
    Value key##_array(kArrayType);                                                                                     \
    for ( const auto &item : var ) {                                                                                   \
        Value item_value;                                                                                              \
        item_value.SetString(item.c_str(), doc.GetAllocator());                                                        \
        key##_array.PushBack(item_value, doc.GetAllocator());                                                          \
    }                                                                                                                  \
    doc.AddMember(#key, key##_array, doc.GetAllocator());

#define ADD_ARRAY_OF_UINTS_MEMBER(var, key)                                                                            \
    Value key##_array(kArrayType);                                                                                     \
    for ( const auto &item : var ) {                                                                                   \
        Value item_value;                                                                                              \
        item_value.SetUint(item);                                                                                      \
        key##_array.PushBack(item_value, doc.GetAllocator());                                                          \
    }                                                                                                                  \
    doc.AddMember(#key, key##_array, doc.GetAllocator());
