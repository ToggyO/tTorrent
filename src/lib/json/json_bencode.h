#pragma once

namespace bencode
{
    void to_json(json& j, const BEncodedValue& value)
    {
        std::visit([&](auto&& arg)
           {
               using T = std::decay_t<decltype(arg)>;
               if constexpr (std::is_same_v<T, Integer> || std::is_same_v<T, std::string>)
               {
                   j = arg;
                   return;
               }
               else if constexpr (std::is_same_v<T, BEncodedList>)
               {
                   auto json_array = json::array();
                   for (const auto& item : std::get<BEncodedList>(value))
                   {
                       json_array.push_back(item);
                   }
                   j = json_array;
                   return;
               }
               else if constexpr (std::is_same_v<T, BEncodedDictionary>)
               {
                   auto json_map = nlohmann::ordered_map<std::string, nlohmann::json>{};
                   for (auto& pair : std::get<BEncodedDictionary>(value))
                   {
                       json_map.emplace(pair.first, pair.second);
                   }
                   j = json_map;
                   return;
               }
           }, value);
    }

    void from_json(const json& j, BEncodedValue& value) {
        // TODO: implement me
//    j.at("name").get_to(p.name);
//    j.at("address").get_to(p.address);
//    j.at("age").get_to(p.age);
    }
}

