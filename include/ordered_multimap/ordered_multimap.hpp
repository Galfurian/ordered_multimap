/// @file ordered_multimap.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief The ordered map class.
///
/// @copyright (c) 2024 This file is distributed under the MIT License.
/// See LICENSE.md for details.
///

#pragma once

#include <list>
#include <map>
#include <vector>

enum : unsigned char {
    ORDERED_MULTIMAP_MAJOR_VERSION = 1, ///< Major version of the library.
    ORDERED_MULTIMAP_MINOR_VERSION = 0, ///< Minor version of the library.
    ORDERED_MULTIMAP_MICRO_VERSION = 0, ///< Micro version of the library.
};

/// @brief This namespace contains the main table class.
namespace ordered_multimap
{

/// @brief A wrapper for a `std::list` container, which uses a `std::map` for accessing the data.
/// @tparam Key the type of the key for building the `std::map`.
/// @tparam Value the value stored inside the `std::list`.
template <typename Key, typename Value> class ordered_multimap_t
{
public:
    /// @brief This stores the key->value association.
    using list_entry_t    = std::pair<Key, Value>;
    /// @brief The actual storage.
    using list_t          = std::list<list_entry_t>;
    /// @brief Iterator for the list, for the user.
    using iterator        = typename list_t::iterator;
    /// @brief Constant iterator for the list, for the user.
    using const_iterator  = typename list_t::const_iterator;
    /// @brief The type of a compatible sort function.
    using sort_function_t = bool (*)(const list_entry_t &, const list_entry_t &);

    /// @brief Construct a new ordered map.
    ordered_multimap_t()
        : list()
        , table()
    {
        // Nothing to do.
    }

    /// @brief Copy constructor.
    /// @param other a reference to the map to copy.
    /// @details I had to define one, otherwise copying this map will screw up
    /// the copy of the iterators contained inside the `std::map`. That is why,
    /// here I copy each individual element of the list and store the iterator,
    /// again.
    ordered_multimap_t(const ordered_multimap_t &other)
        : list()
        , table()
    {
        for (const auto &entry : other.list) {
            table.insert({entry.first, list.insert(list.end(), entry)});
        }
    }

    /// @brief Move constructor.
    /// @param other a reference to the map to move.
    ordered_multimap_t(ordered_multimap_t &&other) noexcept
        : list(std::move(other.list))
        , table(std::move(other.table))
    {
        // Nothing to do.
    }

    /// @brief Move assignment operator.
    /// @param other a reference to the map to move.
    /// @return a reference to the current map.
    auto operator=(ordered_multimap_t &&other) noexcept -> ordered_multimap_t &
    {
        if (this != &other) {
            this->clear();
            list  = std::move(other.list);
            table = std::move(other.table);
        }
        return *this;
    }

    /// @brief Destructor.
    ~ordered_multimap_t() = default;

    /// @brief Clears the content of the map.
    void clear()
    {
        list.clear();
        table.clear();
    }

    /// @brief Returns the number of element in the map.
    /// @return the number of elements.
    auto size() const -> std::size_t { return list.size(); }

    /// @brief Returns an iterator the beginning of the list.
    /// @return an iterator to the beginning of the list.
    auto begin() -> iterator { return list.begin(); }

    /// @brief Returns a const iterator the beginning of the list.
    /// @return an iterator to the beginning of the list.
    auto begin() const -> const_iterator { return list.begin(); }

    /// @brief Returns an iterator the end of the list.
    /// @return an iterator to the end of the list.
    auto end() -> iterator { return list.end(); }

    /// @brief Returns a const iterator the end of the list.
    /// @return an iterator to the end of the list.
    auto end() const -> const_iterator { return list.end(); }

    /// @brief Returns a reverse iterator to the last element in the list.
    /// @return A reverse iterator to the last element.
    auto rbegin() -> typename list_t::reverse_iterator { return list.rbegin(); }

    /// @brief Returns a const reverse iterator to the last element in the list.
    /// @return A const reverse iterator to the last element.
    auto rbegin() const -> typename list_t::const_reverse_iterator { return list.rbegin(); }

    /// @brief Returns a reverse iterator to the position before the first element.
    /// @return A reverse iterator to the position before the first.
    auto rend() -> typename list_t::reverse_iterator { return list.rend(); }

    /// @brief Returns a const reverse iterator to the position before the first element.
    /// @return A const reverse iterator to the position before the first.
    auto rend() const -> typename list_t::const_reverse_iterator { return list.rend(); }

    /// @brief Returns a reference to the first element in the map.
    /// @return An iterator to the first element.
    auto front() -> iterator { return list.begin(); }

    /// @brief Returns a const reference to the first element in the map.
    /// @return A const iterator to the first element.
    auto front() const -> const_iterator { return list.begin(); }

    /// @brief Returns a reference to the last element in the map.
    /// @return An iterator to the last element.
    auto back() -> iterator { return std::prev(list.end()); }

    /// @brief Returns a const reference to the last element in the map.
    /// @return A const iterator to the last element.
    auto back() const -> const_iterator { return std::prev(list.end()); }

    /// @brief Returns a vector containing all keys in the map, in insertion
    /// order.
    /// @return A vector of keys.
    auto keys() const -> std::vector<Key>
    {
        std::vector<Key> result;
        result.reserve(list.size());
        for (const auto &entry : list) {
            result.push_back(entry.first);
        }
        return result;
    }

    /// @brief Returns a vector containing all values in the map, in insertion
    /// order.
    /// @return A vector of values.
    auto values() const -> std::vector<Value>
    {
        std::vector<Value> result;
        result.reserve(list.size());
        for (const auto &entry : list) {
            result.push_back(entry.second);
        }
        return result;
    }

    /// @brief Returns a vector containing all key-value pairs in insertion
    /// order.
    /// @return A vector of key-value pairs.
    auto to_vector() const -> std::vector<list_entry_t> { return std::vector<list_entry_t>(list.begin(), list.end()); }

    /// @brief Sets/updates the `<key,value>` pair inside the map.
    /// @param key the value identifier.
    /// @param value the actual value.
    /// @return the iterator to the newly inserted/updated element in the map.
    auto insert(const Key &key, const Value &value) -> iterator
    {
        // Add the pair to the list.
        iterator it_list = list.insert(list.end(), std::make_pair(key, value));
        // Insert key -> iterator in the multimap.
        table.insert(std::make_pair(key, it_list));
        return it_list;
    }

    /// @brief Constructs a value in-place at the end of the map with the given
    /// key.
    /// @details This function forwards the provided arguments to construct a
    /// `Value` directly within the internal list. It avoids temporary objects
    /// and improves performance for non-trivial value types.
    /// @tparam Args Types of arguments to construct a `Value`.
    /// @param key The key associated with the new value.
    /// @param args Arguments forwarded to construct the `Value`.
    /// @return An iterator to the newly inserted element.
    template <typename... Args> auto emplace(const Key &key, Args &&...args) -> iterator
    {
        iterator it_list = list.emplace(
            list.end(), std::piecewise_construct, std::forward_as_tuple(key),
            std::forward_as_tuple(std::forward<Args>(args)...));
        table.insert(std::make_pair(key, it_list));
        return it_list;
    }

    /// @brief Updates all values associated with the given key to the new
    /// value.
    /// @details This function finds all entries matching the key and updates
    /// their values in-place. If no such entries exist, a new one is appended
    /// at the end.
    /// @param key The key to update.
    /// @param value The new value to assign.
    /// @return An iterator to the first updated or newly inserted element.
    auto update(const Key &key, const Value &value) -> iterator
    {
        auto range = table.equal_range(key);

        if (range.first == range.second) {
            // No match: behave like insert.
            iterator it_list = list.insert(list.end(), std::make_pair(key, value));
            table.insert(std::make_pair(key, it_list));
            return it_list;
        }

        // Otherwise, update all matching values.
        iterator first_updated = range.first->second;
        for (auto it = range.first; it != range.second; ++it) {
            it->second->second = value;
        }
        return first_updated;
    }

    /// @brief Erases the elment from the list, and returns an iteator to the
    /// same position in the list (i.e., the elment after the one removed).
    /// @param key the key of the element to remove.
    /// @return an iterator to the same position in the list.
    auto erase(const Key &key) -> iterator
    {
        auto range = table.equal_range(key);
        if (range.first == range.second) {
            return list.end();
        }
        iterator next = list.end();
        for (auto it = range.first; it != range.second; ++it) {
            if (next == list.end()) {
                next = std::next(it->second);
            }
            list.erase(it->second);
        }
        table.erase(range.first, range.second);
        return next;
    }

    /// @brief Erases the elment from the list, and returns an iteator to the
    /// same position in the list (i.e., the elment after the one removed).
    /// @param it_list the iterator of the element to remove.
    /// @return an iterator to the same position in the list.
    auto erase(iterator it_list) -> iterator
    {
        table_iterator it_table = table.find(it_list->first);
        if (it_table == table.end()) {
            return list.end();
        }
        ++it_list;
        list.erase(it_table->second);
        table.erase(it_table);
        return it_list;
    }

    /// @brief Erases a single element that matches the given key and value.
    /// @details This function removes only the first occurrence of the
    /// specified key-value pair. If no such pair is found, the function does
    /// nothing.
    /// @param key The key to search for.
    /// @param value The value to match against.
    /// @return The number of elements removed (0 or 1).
    auto erase(const Key &key, const Value &value) -> std::size_t
    {
        auto range = table.equal_range(key);
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second->second == value) {
                list.erase(it->second);
                table.erase(it);
                return 1;
            }
        }
        return 0;
    }

    /// @brief Returns an iterator to the element in the given position.
    /// @param position the position of the element to retrieve.
    /// @return an iterator to the element, or the end of the list if not found.
    auto at(std::size_t position) -> iterator
    {
        iterator itr = list.begin();
        std::advance(itr, std::min(position, list.size()));
        return itr;
    }

    /// @brief Returns an iterator to the element in the given position.
    /// @param position the position of the element to retrieve.
    /// @return an iterator to the element, or the end of the list if not found.
    auto at(std::size_t position) const -> const_iterator
    {
        const_iterator itr = list.begin();
        std::advance(itr, std::min(position, list.size()));
        return itr;
    }

    /// @brief Returns the index of the given iterator in the internal list.
    /// @details This function returns the zero-based position of the provided
    /// iterator relative to the beginning of the list. If the iterator is
    /// invalid or not found, the behavior is undefined.
    /// @param it The iterator to locate.
    /// @return The index of the iterator in the list.
    auto index_of(const const_iterator &it) const -> std::size_t { return std::distance(list.begin(), it); }

    /// @brief Returns an iterator to the element associated with the given key.
    /// @param key the key of the element to search for.
    /// @return an iterator to the element, or the end of the list if not found.
    auto find(const Key &key) -> iterator
    {
        table_iterator itr = table.find(key);
        if (itr == table.end()) {
            return list.end();
        }
        return itr->second;
    }

    /// @brief Returns an iterator to the element associated with the given key.
    /// @param key the key of the element to search for.
    /// @return an iterator to the element, or the end of the list if not found.
    auto find(const Key &key) const -> const_iterator
    {
        table_const_iterator itr = table.find(key);
        if (itr == table.end()) {
            return list.end();
        }
        return itr->second;
    }

    /// @brief Checks whether at least one element with the given key exists.
    /// @details This function is a shorthand for `find(key) != end()`. It is
    /// useful for making code more expressive and readable.
    /// @param key The key to check.
    /// @return True if the key exists, false otherwise.
    auto has(const Key &key) const -> bool { return table.find(key) != table.end(); }

    /// @brief Counts the number of elements associated with the given key.
    /// @details This function returns how many entries in the map match the
    /// given key. It uses the internal multimap to retrieve the range
    /// efficiently.
    /// @param key The key to count occurrences for.
    /// @return The number of elements associated with the key.
    auto count(const Key &key) const -> std::size_t
    {
        auto range = table.equal_range(key);
        return std::distance(range.first, range.second);
    }

    /// @brief Sorts the internal list.
    /// @param fun the sorting function.
    void sort(const sort_function_t &fun) { list.sort(fun); }

    /// @brief Returns a range of iterators to the elements with the given key.
    /// @details This function allows traversal over all elements that match the
    /// given key, preserving their insertion order.
    /// @param key The key to search for.
    /// @return A pair of iterators [begin, end) to elements in the list that match the key.
    auto equal_range(const Key &key) const -> std::pair<const_iterator, const_iterator>
    {
        auto range = table.equal_range(key);
        if (range.first == range.second) {
            return {list.end(), list.end()};
        }

        const_iterator begin = range.first->second;
        const_iterator end   = std::next(range.second == table.end() ? list.end() : range.second->second);
        return {begin, end};
    }

    /// @brief Returns a mutable range of iterators to the elements with the
    /// given key.
    /// @details This version of equal_range allows modifying the values within
    /// the range. The returned iterators span all matching elements in
    /// insertion order.
    /// @param key The key to search for.
    /// @return A pair of mutable iterators [begin, end) to elements matching the key.
    auto equal_range(const Key &key) -> std::pair<iterator, iterator>
    {
        auto range = table.equal_range(key);
        if (range.first == range.second) {
            return {list.end(), list.end()};
        }

        iterator begin = range.first->second;
        iterator end   = std::next(range.second == table.end() ? list.end() : range.second->second);
        return {begin, end};
    }

    /// @brief Merges the contents of another ordered_multimap_t into this one.
    /// @details All elements from the other map are inserted at the end of this
    /// map. The other map is cleared after the operation. Insertion order is
    /// preserved.
    /// @param other The other ordered_multimap_t to merge (rvalue).
    void merge(ordered_multimap_t &&other)
    {
        for (auto it = other.list.begin(); it != other.list.end(); ++it) {
            iterator new_it = list.insert(list.end(), std::move(*it));
            table.insert(std::make_pair(new_it->first, new_it));
        }
        other.clear();
    }

    /// @brief Extracts and removes all values associated with the given key.
    /// @details This function collects all values associated with the given
    /// key, removes them from the map, and returns them in order of insertion.
    /// @param key The key to extract.
    /// @return A vector containing all values that were associated with the key.
    auto extract(const Key &key) -> std::vector<Value>
    {
        std::vector<Value> result;
        auto range = table.equal_range(key);
        for (auto it = range.first; it != range.second; ++it) {
            result.push_back(std::move(it->second->second));
            list.erase(it->second);
        }
        table.erase(range.first, range.second);
        return result;
    }

    /// @brief Assign operator.
    /// @details I had to define one, otherwise copying this map will screw up
    /// the copy of the iterators contained inside the `std::map`. That is why,
    /// here I copy each individual element of the list and store the iterator,
    /// again.
    /// @param other a reference to the map to copy.
    /// @return a reference to the current map.
    auto operator=(const ordered_multimap_t &other) -> ordered_multimap_t &
    {
        if (this != &other) {
            this->clear();
            for (const auto &entry : other.list) {
                table.insert({entry.first, list.insert(list.end(), entry)});
            }
        }
        return *this;
    }

private:
    /// @brief Type of the map.
    using table_t              = std::multimap<Key, iterator>;
    using table_iterator       = typename table_t::iterator;
    using table_const_iterator = typename table_t::const_iterator;
    /// @brief The list containing the actual data.
    list_t list;
    /// @brief A table for easy access to the data by using a key.
    table_t table;
};

} // namespace ordered_multimap
