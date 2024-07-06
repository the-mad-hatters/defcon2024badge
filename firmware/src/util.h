#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <sstream>
#include <set>
#include <map>

/**
 * @brief A simple bidirectional map.
 *
 * @tparam X The left type.
 * @tparam Y The right type.
 */
template <typename X, typename Y> struct BiMap {
    BiMap() = default;
    BiMap(std::initializer_list<std::pair<X, Y>> pairs) {
        for (const auto &pair : pairs) {
            insert(pair.first, pair.second);
        }
    }

    /**
     * @brief Insert a pair into the map.
     *
     * @param left The left value.
     * @param right The right value.
     */
    void insert(const X &left_val, const Y &right_val) {
        if (left.forward.find(left_val) != left.forward.end() || right.forward.find(right_val) != right.forward.end()) {
            throw std::runtime_error("Duplicate left or right value.");
        }
        left.forward[left_val]   = right_val;
        right.forward[right_val] = left_val;
    }

    // clang-format off

    // Nested class for left-to-right mapping
    class {
      public:
        const Y &operator[](const X &left) const { return forward.at(left); }
        const Y &operator()(const X &left) const { return forward.at(left); }

        auto begin() const { return forward.begin(); }
        auto end() const { return forward.end(); }

        std::set <X> keys() const {
            std::set<X> keys;
            std::transform(forward.begin(), forward.end(), std::inserter(keys, keys.begin()),
                           [](const auto &pair) { return pair.first; });
            return keys;
        }

      private:
        std::map<X, Y> forward;
        friend class BiMap;
    } left;

    // Nested class for right-to-left mapping
    class {
      public:
        const X &operator[](const Y &right) const { return forward.at(right); }
        const X &operator()(const Y &right) const { return forward.at(right); }

        auto begin() const { return forward.begin(); }
        auto end() const { return forward.end(); }

        std::set <Y> keys() const {
            std::set<Y> keys;
            std::transform(forward.begin(), forward.end(), std::inserter(keys, keys.begin()),
                           [](const auto &pair) { return pair.first; });
            return keys;
        }

      private:
        std::map<Y, X> forward;
        friend class BiMap;
    } right;

    // clang-format on
};

// Join function that takes a container and a delimiter
template <typename Container> std::string join(const Container &elements, const std::string &delimiter) {
    std::ostringstream os;
    auto begin = std::begin(elements);
    auto end   = std::end(elements);

    if (begin != end) {
        std::copy(begin, std::prev(end), std::ostream_iterator<typename Container::value_type>(os, delimiter.c_str()));
        os << *std::prev(end);
    }

    return os.str();
}

#endif // UTIL_H
