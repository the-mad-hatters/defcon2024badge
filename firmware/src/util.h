#ifndef UTIL_H
#define UTIL_H

#include <map>

/**
 * @brief A simple bidirectional map.
 *
 * @tparam X The left type.
 * @tparam Y  The right type.
 */
template <typename X, typename Y> class BiMap {
  public:
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
    void insert(const X &left, const Y &right) {
        if (forward.find(left) != forward.end() || reverse.find(right) != reverse.end()) {
            throw std::runtime_error("Duplicate left or right value.");
        }
        forward[left]  = right;
        reverse[right] = left;
    }

    /**
     * @brief Get the right value for a left value.
     *
     * @param left The left value.
     * @return The right value.
     */
    Y right(const X &left) const {
        auto it = forward.find(left);
        if (it != forward.end()) {
            return it->second;
        }
        throw std::runtime_error("left value not found.");
    }

    /**
     * @brief Get the left value for a right value.
     *
     * @param right The right value.
     * @return The left value.
     */
    X left(const Y &right) const {
        auto it = reverse.find(right);
        if (it != reverse.end()) {
            return it->second;
        }
        throw std::runtime_error("right value not found.");
    }

  private:
    std::map<X, Y> forward;
    std::map<Y, X> reverse;
};

#endif // UTIL_H
