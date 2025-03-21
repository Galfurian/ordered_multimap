/// @file test_ordered_multimap.cpp
/// @brief Comprehensive unit tests for ordered_multimap_t.
///
/// @details This test covers insertion, duplicate handling,
/// lookup, iteration, sorting, erasure (by key and iterator),
/// size tracking, and iterator validity. The test is meant to
/// run with `ctest` using plain assertions (no external framework).
///

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#include "ordered_multimap/ordered_multimap.hpp"

using Table = ordered_multimap::ordered_multimap_t<std::string, int>;

void test_insertion_and_order()
{
    std::cout << ">>> test_insertion_and_order\n";

    Table table;
    table.insert("a", 1);
    table.insert("b", 2);
    table.insert("a", 3);
    table.insert("c", 4);

    std::ostringstream oss;
    for (const auto &entry : table) {
        oss << entry.first << ":" << entry.second << " ";
    }
    assert(oss.str() == "a:1 b:2 a:3 c:4 ");
}

void test_duplicate_keys()
{
    std::cout << ">>> test_duplicate_keys\n";

    Table table;
    table.insert("x", 10);
    table.insert("x", 11);
    table.insert("x", 12);

    int count = 0;
    for (const auto &entry : table) {
        if (entry.first == "x")
            ++count;
    }
    assert(count == 3);
}

void test_find_and_erase_by_iterator()
{
    std::cout << ">>> test_find_and_erase_by_iterator\n";

    Table table;
    table.insert("a", 1);
    table.insert("b", 2);
    table.insert("b", 3);
    table.insert("c", 4);

    auto it = table.find("b");
    assert(it != table.end());
    assert(it->first == "b");

    table.erase(it); // should erase first "b"

    int b_count = 0;
    for (const auto &entry : table) {
        if (entry.first == "b")
            ++b_count;
    }
    assert(b_count == 1);
}

void test_erase_by_key()
{
    std::cout << ">>> test_erase_by_key\n";

    Table table;
    table.insert("d", 1);
    table.insert("d", 2);
    table.insert("e", 3);

    table.erase("d");

    for (const auto &entry : table) {
        assert(entry.first != "d");
    }

    assert(table.size() == 1);
}

void test_sort_ascending_and_descending()
{
    std::cout << ">>> test_sort_ascending_and_descending\n";

    Table table;
    table.insert("c", 3);
    table.insert("a", 1);
    table.insert("b", 2);
    table.insert("a", 4);

    auto compare_asc = [](const Table::list_entry_t &lhs, const Table::list_entry_t &rhs) {
        return lhs.first < rhs.first;
    };

    auto compare_desc = [](const Table::list_entry_t &lhs, const Table::list_entry_t &rhs) {
        return lhs.first > rhs.first;
    };

    table.sort(compare_asc);

    std::ostringstream oss1;
    for (const auto &entry : table) {
        oss1 << entry.first << ":" << entry.second << " ";
    }
    assert(oss1.str() == "a:1 a:4 b:2 c:3 ");

    table.sort(compare_desc);

    std::ostringstream oss2;
    for (const auto &entry : table) {
        oss2 << entry.first << ":" << entry.second << " ";
    }
    assert(oss2.str() == "c:3 b:2 a:1 a:4 ");
}

void test_iterator_validity()
{
    std::cout << ">>> test_iterator_validity\n";

    Table table;
    table.insert("x", 100);
    table.insert("y", 200);
    table.insert("z", 300);

    auto it = table.find("y");
    assert(it != table.end());
    assert(it->second == 200);

    table.sort([](const Table::list_entry_t &a, const Table::list_entry_t &b) { return a.second < b.second; });

    // iterator should still be valid and point to the same element
    assert(it->first == "y");
    assert(it->second == 200);
}

void test_clear_and_reuse()
{
    std::cout << ">>> test_clear_and_reuse\n";

    Table table;
    table.insert("p", 9);
    table.insert("q", 8);

    table.clear();
    assert(table.size() == 0);
    assert(table.begin() == table.end());

    table.insert("x", 1);
    table.insert("x", 2);
    assert(table.size() == 2);
}

void test_at_index_access()
{
    std::cout << ">>> test_at_index_access\n";

    Table table;
    table.insert("a", 1);
    table.insert("b", 2);
    table.insert("c", 3);

    auto it = table.at(1);
    assert(it != table.end());
    assert(it->first == "b");

    auto invalid = table.at(999);
    assert(invalid == table.end());
}

void test_copy_and_move()
{
    std::cout << ">>> test_copy_and_move\n";

    Table original;
    original.insert("a", 10);
    original.insert("b", 20);

    Table copy = original;
    assert(copy.size() == 2);
    assert(copy.begin()->first == "a");

    Table moved = std::move(copy);
    assert(moved.size() == 2);
    assert(copy.size() == 0); // Optional: if `copy.clear()` on move is enforced
}

void test_erase_all_by_iterator()
{
    std::cout << ">>> test_erase_all_by_iterator\n";

    Table table;
    for (int i = 0; i < 5; ++i)
        table.insert("k" + std::to_string(i), i);

    auto it = table.begin();
    while (it != table.end()) {
        it = table.erase(it);
    }

    assert(table.size() == 0);
}

void test_sort_by_value()
{
    std::cout << ">>> test_sort_by_value\n";

    Table table;
    table.insert("z", 5);
    table.insert("y", 1);
    table.insert("x", 3);

    table.sort([](const Table::list_entry_t &a, const Table::list_entry_t &b) { return a.second < b.second; });

    std::ostringstream oss;
    for (const auto &entry : table)
        oss << entry.second << " ";
    assert(oss.str() == "1 3 5 ");
}

void test_iterator_consistency()
{
    std::cout << ">>> test_iterator_consistency\n";

    Table table;
    table.insert("a", 1);
    auto it = table.insert("b", 2);
    table.insert("c", 3);

    assert(it->second == 2);

    table.sort([](const Table::list_entry_t &a, const Table::list_entry_t &b) { return a.first > b.first; });

    assert(it->first == "b");
    assert(it->second == 2);
}

void test_equal_range()
{
    std::cout << ">>> test_equal_range\n";

    Table table;
    table.insert("x", 10);
    table.insert("x", 11);
    table.insert("y", 20);
    table.insert("x", 12);

    auto [begin, end] = table.equal_range("x");

    std::vector<int> values;
    for (auto it = begin; it != end; ++it) {
        assert(it->first == "x");
        values.push_back(it->second);
    }

    assert(values.size() == 3);
    assert(values[0] == 10);
    assert(values[1] == 11);
    assert(values[2] == 12);

    // Also check for key with no match
    auto [nbegin, nend] = table.equal_range("z");
    assert(nbegin == table.end());
    assert(nend == table.end());
}

void test_count()
{
    std::cout << ">>> test_count\n";

    Table table;
    table.insert("a", 1);
    table.insert("a", 2);
    table.insert("b", 3);
    table.insert("a", 4);
    table.insert("c", 5);

    assert(table.count("a") == 3);
    assert(table.count("b") == 1);
    assert(table.count("c") == 1);
    assert(table.count("z") == 0);
}

void test_has()
{
    std::cout << ">>> test_has\n";

    Table table;
    table.insert("x", 42);
    table.insert("y", 13);

    assert(table.has("x"));
    assert(table.has("y"));
    assert(!table.has("z"));

    table.erase("x");
    assert(!table.has("x"));
}

struct Thing {
    int a, b;
    Thing(int x, int y)
        : a(x)
        , b(y)
    {
    }
    bool operator==(const Thing &other) const { return a == other.a && b == other.b; }
};

void test_emplace()
{
    std::cout << ">>> test_emplace\n";

    ordered_multimap::ordered_multimap_t<std::string, Thing> table;
    table.emplace("alpha", 1, 2);
    table.emplace("beta", 3, 4);

    auto it1 = table.find("alpha");
    auto it2 = table.find("beta");

    assert(it1 != table.end());
    assert(it2 != table.end());
    assert(it1->second == Thing(1, 2));
    assert(it2->second == Thing(3, 4));
}

void test_reverse_iteration()
{
    std::cout << ">>> test_reverse_iteration\n";

    Table table;
    table.insert("a", 1);
    table.insert("b", 2);
    table.insert("c", 3);

    std::vector<std::string> keys;
    for (auto it = table.rbegin(); it != table.rend(); ++it) {
        keys.push_back(it->first);
    }

    assert(keys.size() == 3);
    assert(keys[0] == "c");
    assert(keys[1] == "b");
    assert(keys[2] == "a");
}

void test_erase_key_value()
{
    std::cout << ">>> test_erase_key_value\n";

    Table table;
    table.insert("a", 1);
    table.insert("a", 2);
    table.insert("a", 3);

    assert(table.count("a") == 3);
    auto removed = table.erase("a", 2);
    assert(removed == 1);
    assert(table.count("a") == 2);

    // Ensure remaining values are correct.
    std::vector<int> values;
    for (const auto &entry : table) {
        if (entry.first == "a") {
            values.push_back(entry.second);
        }
    }

    assert((values == std::vector<int>{1, 3}));

    // Try erasing a value that doesn't exist.
    removed = table.erase("a", 999);
    assert(removed == 0);
    assert(table.count("a") == 2);
}

void test_index_of()
{
    std::cout << ">>> test_index_of\n";

    Table table;
    auto it0 = table.insert("a", 10); // index 0
    auto it1 = table.insert("b", 20); // index 1
    auto it2 = table.insert("c", 30); // index 2

    assert(table.index_of(it0) == 0);
    assert(table.index_of(it1) == 1);
    assert(table.index_of(it2) == 2);
}

void test_merge()
{
    std::cout << ">>> test_merge\n";

    Table table1;
    table1.insert("a", 1);
    table1.insert("b", 2);

    Table table2;
    table2.insert("c", 3);
    table2.insert("a", 4);

    table1.merge(std::move(table2));

    // Check merged content and order.
    std::vector<std::string> keys;
    for (const auto &entry : table1) {
        keys.push_back(entry.first);
    }

    assert((keys == std::vector<std::string>{"a", "b", "c", "a"}));
    assert(table1.count("a") == 2);
    assert(table1.count("b") == 1);
    assert(table1.count("c") == 1);

    // Original map should be empty.
    assert(table2.size() == 0);
}

void test_extract()
{
    std::cout << ">>> test_extract\n";

    Table table;
    table.insert("x", 100);
    table.insert("y", 200);
    table.insert("x", 101);
    table.insert("x", 102);

    assert(table.count("x") == 3);

    auto values = table.extract("x");
    assert(values.size() == 3);
    assert(values[0] == 100);
    assert(values[1] == 101);
    assert(values[2] == 102);

    assert(table.count("x") == 0);
    assert(table.size() == 1); // only "y" remains
}

void test_update_overwrite()
{
    std::cout << ">>> test_update_overwrite\n";

    Table table;
    table.insert("a", 1);
    table.insert("a", 2);
    table.insert("a", 3);

    auto it = table.update("a", 99);

    assert(it != table.end());
    assert(it->first == "a");
    assert(it->second == 99);

    int count = 0;
    for (const auto &entry : table) {
        if (entry.first == "a") {
            assert(entry.second == 99);
            ++count;
        }
    }
    assert(count == 3);
}

void test_front_and_back()
{
    std::cout << ">>> test_front_and_back\n";

    Table table;
    table.insert("x", 10);
    table.insert("y", 20);
    table.insert("z", 30);

    auto it_front = table.front();
    auto it_back  = table.back();

    assert(it_front != table.end());
    assert(it_back != table.end());

    assert(it_front->first == "x");
    assert(it_front->second == 10);

    assert(it_back->first == "z");
    assert(it_back->second == 30);
}

void test_keys_and_values()
{
    std::cout << ">>> test_keys_and_values\n";

    Table table;
    table.insert("alpha", 1);
    table.insert("beta", 2);
    table.insert("gamma", 3);

    auto keys = table.keys();
    auto vals = table.values();

    assert((keys == std::vector<std::string>{"alpha", "beta", "gamma"}));
    assert((vals == std::vector<int>{1, 2, 3}));
}

void test_to_vector()
{
    std::cout << ">>> test_to_vector\n";

    Table table;
    table.insert("a", 1);
    table.insert("b", 2);
    table.insert("a", 3);

    auto vec = table.to_vector();

    assert(vec.size() == 3);
    assert(vec[0] == std::make_pair(std::string("a"), 1));
    assert(vec[1] == std::make_pair(std::string("b"), 2));
    assert(vec[2] == std::make_pair(std::string("a"), 3));
}

int main()
{
    std::cout << "Running ordered_multimap_t tests...\n";

    test_insertion_and_order();
    test_duplicate_keys();
    test_find_and_erase_by_iterator();
    test_erase_by_key();
    test_sort_ascending_and_descending();
    test_iterator_validity();
    test_clear_and_reuse();
    test_at_index_access();
    test_copy_and_move();
    test_erase_all_by_iterator();
    test_sort_by_value();
    test_iterator_consistency();
    test_count();
    test_has();
    test_emplace();
    test_reverse_iteration();
    test_erase_key_value();
    test_index_of();
    test_merge();
    test_extract();
    test_update_overwrite();
    test_front_and_back();
    test_keys_and_values();
    test_to_vector();

    std::cout << "All tests passed!\n";
    return 0;
}
