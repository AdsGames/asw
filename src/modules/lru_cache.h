/// @file lru_cache.h
/// @brief Bounded cache that drops the least recently used entries. Private
/// to the library.

#ifndef ASW_SRC_LRU_CACHE_H
#define ASW_SRC_LRU_CACHE_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

namespace asw::detail {

/// @brief A hash map with a size limit. When full, the least recently used
/// half is dropped, so entries in use every frame stay cached.
///
/// @details Hash and Equal may be transparent, so find() can take a key view
/// that does not allocate.
///
template <typename Key, typename Value, typename Hash, typename Equal> class LruCache {
public:
    explicit LruCache(std::size_t limit)
        : _limit(limit)
    {
    }

    /// @brief Find an entry and mark it used.
    /// @return The value, or nullptr when not cached.
    template <typename K> Value* find(const K& key)
    {
        auto it = _map.find(key);
        if (it == _map.end()) {
            return nullptr;
        }
        it->second.last_used = ++_clock;
        return &it->second.value;
    }

    /// @brief Add or replace an entry, dropping old entries first when full.
    /// @return The stored value.
    Value& insert(Key key, Value value)
    {
        if (_map.size() >= _limit) {
            evict();
        }
        auto& entry
            = _map.insert_or_assign(std::move(key), Entry { std::move(value), 0 }).first->second;
        entry.last_used = ++_clock;
        return entry.value;
    }

    void clear()
    {
        _map.clear();
    }

    std::size_t size() const
    {
        return _map.size();
    }

private:
    struct Entry {
        Value value;
        uint64_t last_used;
    };

    // Drop the least recently used half. Runs once per limit / 2 inserts, so
    // the cost per insert stays constant.
    void evict()
    {
        // Keep the newest half, rounded down, so at least one entry goes
        const std::size_t keep = _map.size() / 2;
        if (keep == 0) {
            _map.clear();
            return;
        }

        _stamps.clear();
        _stamps.reserve(_map.size());
        for (const auto& [key, entry] : _map) {
            _stamps.push_back(entry.last_used);
        }

        // Stamps are unique, so the ones below the cutoff are exactly the
        // size - keep oldest
        const auto cutoff_it = _stamps.begin() + static_cast<std::ptrdiff_t>(_stamps.size() - keep);
        std::nth_element(_stamps.begin(), cutoff_it, _stamps.end());
        const uint64_t cutoff = *cutoff_it;
        std::erase_if(_map, [cutoff](const auto& item) { return item.second.last_used < cutoff; });
    }

    std::unordered_map<Key, Entry, Hash, Equal> _map;
    std::vector<uint64_t> _stamps;
    std::size_t _limit;
    uint64_t _clock { 0 };
};

/// @brief Combine a hash into a seed, as boost::hash_combine does.
inline void hash_combine(std::size_t& seed, std::size_t hash)
{
    seed ^= hash + 0x9e3779b9 + ((seed << 6) + (seed >> 2));
}

} // namespace asw::detail

#endif // ASW_SRC_LRU_CACHE_H
