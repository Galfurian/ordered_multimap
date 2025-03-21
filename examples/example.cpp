/// @file example.cpp
/// @brief A couple of examples for the ordered multimap.
///
/// @details Demonstrates usage of ordered_multimap_t, including insertion of
/// duplicate keys, erasure, sorting, and iteration.

#include <iostream>
#include <sstream>

#include "ordered_multimap/ordered_multimap.hpp"

using Table = ordered_multimap::ordered_multimap_t<std::string, int>;

inline void print(const Table &table)
{
    std::cout << "{ ";
    for (const auto &it : table) {
        std::cout << "[" << it.first << "](" << it.second << ") ";
    }
    std::cout << "}\n";
}

/// @brief Comparison function to sort entries by decreasing key order.
auto compare_gt(const Table::list_entry_t &lhs, const Table::list_entry_t &rhs) -> bool
{
    return lhs.first > rhs.first;
}

/// @brief Comparison function to sort entries by increasing key order.
auto compare_lt(const Table::list_entry_t &lhs, const Table::list_entry_t &rhs) -> bool
{
    return lhs.first < rhs.first;
}

auto main(int /*unused*/, char * /*unused*/[]) -> int
{
    Table table;

    std::cout << "== Insert multiple values with same keys\n";
    table.insert("a", 1);
    table.insert("b", 2);
    table.insert("a", 3); // duplicate key
    table.insert("c", 4);
    table.insert("b", 5); // duplicate key
    print(table);

    std::cout << "== Sort by key descending\n";
    table.sort(compare_gt);
    print(table);

    std::cout << "== Sort by key ascending\n";
    table.sort(compare_lt);
    print(table);

    std::cout << "== Erase all entries with key 'a'\n";
    table.erase("a");
    print(table);

    std::cout << "== Erase a single entry with iterator (key 'b')\n";
    auto it_erase = table.find("b");
    if (it_erase != table.end()) {
        table.erase(it_erase);
    }
    print(table);

    std::cout << "== Add more values and print again\n";
    table.insert("d", 9);
    table.insert("d", 10);
    table.insert("e", 11);
    print(table);

    return 0;
}
