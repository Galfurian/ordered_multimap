# Ordered Multimap

[![Ubuntu](https://github.com/Galfurian/ordered_multimap/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/Galfurian/ordered_multimap/actions/workflows/ubuntu.yml)
[![Windows](https://github.com/Galfurian/ordered_multimap/actions/workflows/windows.yml/badge.svg)](https://github.com/Galfurian/ordered_multimap/actions/workflows/windows.yml)
[![MacOS](https://github.com/Galfurian/ordered_multimap/actions/workflows/macos.yml/badge.svg)](https://github.com/Galfurian/ordered_multimap/actions/workflows/macos.yml)
[![Documentation](https://github.com/Galfurian/ordered_multimap/actions/workflows/documentation.yml/badge.svg)](https://github.com/Galfurian/ordered_multimap/actions/workflows/documentation.yml)

A C++ container that behaves like a multimap but **preserves the order of insertion** and offers **efficient key lookup**.

## Overview

The **`ordered_multimap`** library provides an `ordered_multimap_t<Key, Value>` class template that combines:

- A `std::list` to preserve insertion order.
- A `std::multimap<Key, iterator>` for efficient access.

It supports **duplicate keys**, stable iterators, and ordered traversal.

## Features

- **Insertion-order iteration**
- **Duplicate key support** (like `std::multimap`)
- **Efficient key lookup** via internal `std::multimap`
- **Stable iterators** — safe during most operations
- **Custom sorting** with user-defined comparators
- **Rich API** including:
  - `insert`, `emplace`, `erase`, `find`, `count`, `has`
  - `equal_range`, `update`, `extract`, `merge`
  - `front`, `back`, `keys`, `values`, `to_vector`
- **Full iterator support**: `begin`, `end`, `rbegin`, `rend`

## Summary of Trade-Offs

| Feature                   | std::multimap | std::unordered_multimap | Your ordered_multimap_t         |
|---------------------------|---------------|-------------------------|---------------------------------|
| Maintains key order       | V             | X                       | Optional (sort)                 |
| Maintains insertion order | X             | X                       | V                               |
| Allows duplicate keys     | V             | V                       | V                               |
| Lookup time               | O(log N)      | O(1) avg                | O(log N) via internal map       |
| Insert/remove performance | O(log N)      | O(1) avg                | O(1) for list, O(log N) for map |
| Iterator stability        | X             | X                       | V                               |
| Custom sorting            | X             | X                       | V                               |

## Requirements

- **C++11** or later.
- Standard C++ libraries (`<list>`, `<map>`, `<utility>`, etc.)

## Installation

To use the `ordered_multimap_t` class in your project, include the header file:

```c++
#include "ordered_multimap.hpp"
```

Ensure the `ordered_multimap.hpp` file is in your project's include path.

## Usage

Below is an example showcasing basic usage of `ordered_multimap_t`:

```c++
#include "ordered_multimap.hpp"
#include <iostream>

int main()
{
    ordered_multimap::ordered_multimap_t<int, std::string> omap;

    omap.insert(1, "one");
    omap.insert(2, "two");
    omap.insert(1, "uno"); // duplicate key
    omap.insert(3, "three");

    // Iteration preserves insertion order.
    for (const auto &entry : omap)
    {
        std::cout << entry.first << ": " << entry.second << '\n';
    }

    // Access all values for a key.
    auto range = omap.equal_range(1);
    for (auto it = range.first; it != range.second; ++it)
    {
        std::cout << "1 → " << it->second << '\n';
    }

    // Replace all values for key 2 with a single new value.
    omap.update(2, "dos");

    // Extract and remove all values for a key.
    auto removed = omap.extract(1);
    std::cout << "Removed " << removed.size() << " values for key 1.\n";
}
```

Output:

```bash
1: One
2: Two
3: Three

Found: 2 -> Two
```

## License

This project is licensed under the **MIT License**.

Copyright (c) 2024 Enrico Fraccaroli <enry.frak@gmail.com>.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.