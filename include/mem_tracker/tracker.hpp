#ifndef MEM_TRACKER_TRACKER_HPP
#define MEM_TRACKER_TRACKER_HPP

#include <optional>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <memory_resource>
#include <experimental/memory>
#include "alloc_type.hpp"
#include "track_type.hpp"
#include "alloc_state.hpp"

namespace mem_tracker {
  using pointers_map_key = std::pair<std::experimental::observer_ptr<void>, track_type>;

  struct pointers_map_key_hash
  {
    std::size_t operator() (pointers_map_key const& pair) const
    {
      return std::hash<std::experimental::observer_ptr<void>>()(pair.first) ^
             (std::hash<track_type>()(pair.second) << 1u);
    }
  };

  enum class num_val_t : std::size_t {};

  template<std::size_t S, std::align_val_t A = std::align_val_t{alignof(std::max_align_t)}>
  class tracker {
  public:
    using buffer = std::array<std::byte, S>;
    using container =
    std::pmr::unordered_multimap<
        pointers_map_key,
        std::tuple<num_val_t, std::size_t, std::align_val_t, alloc_type, alloc_state, std::string_view>,
        pointers_map_key_hash>;
    using const_iterator = typename container::const_iterator;

    std::size_t allocations() const { return allocations_; }
    std::size_t deallocations() const { return deallocations_; }
    std::size_t bytes_allocated() const { return bytes_allocated_; }
    std::size_t bytes_deallocated() const { return bytes_deallocated_; }
    std::size_t constructions() const { return constructions_; }
    std::size_t destructions() const { return destructions_; }

    const_iterator begin() { return std::begin(pointers_map_); }
    const_iterator end() { return std::end(pointers_map_); }

    void track_allocate(void* ptr, std::size_t num, std::size_t size,
                        std::align_val_t align, alloc_type atype, std::string_view type)
    {
      auto p = std::experimental::make_observer(ptr);
      if (auto it = pointers_map_.find(std::pair{p, track_type::allocation});
          it != std::end(pointers_map_) && std::get<alloc_state>(it->second) == alloc_state ::allocated) {
        std::get<alloc_state>(it->second) = alloc_state::already_allocated;
      }
      else {
        pointers_map_.emplace(std::pair{std::experimental::make_observer(ptr), track_type::allocation},
                              std::make_tuple(num_val_t{num}, size, align, atype, alloc_state::allocated, type));
      }
      allocations_++;
      bytes_allocated_ += num * size;
    }

    void track_construct(void* ptr, std::size_t num, std::size_t size,
                         std::align_val_t align, alloc_type atype, std::string_view type)
    {
      auto p = std::experimental::make_observer(ptr);
      if (auto it = pointers_map_.find(std::pair{p, track_type::construction});
          it != std::end(pointers_map_) && std::get<alloc_state>(it->second) == alloc_state ::constructed) {
        std::get<alloc_state>(it->second) = alloc_state::already_constructed;
      }
      else {
        pointers_map_.emplace(std::pair{p, track_type::construction},
                              std::make_tuple(num_val_t{num}, size, align, atype, alloc_state::constructed, type));
      }
      constructions_++;
    }

    void track_destroy(void* ptr, std::size_t num, std::size_t size,
                         std::align_val_t align, alloc_type atype, std::string_view type)
    {
      auto p = std::experimental::make_observer(ptr);
      auto rng = pointers_map_.equal_range({std::experimental::make_observer(ptr), track_type::construction});
      if (rng.first != rng.second) {
        for (auto it = rng.first; it != rng.second; ++it) {
          auto state = std::get<alloc_state>(it->second);

          if (state == alloc_state::constructed) {
            std::get<alloc_state>(it->second) = alloc_state::destroyed;
            destructions_++;
            break;
          }
          if (std::next(it) == rng.second &&
              (state == alloc_state::destroyed || state == alloc_state::destroyed_not_match)) {
            std::get<alloc_state>(it->second) = alloc_state::already_destroyed;
            destructions_++;
            break;
          }
        }
      }
      else {
        pointers_map_.emplace(std::pair{p, track_type::construction},
                              std::make_tuple(num_val_t{num}, size, align, atype, alloc_state::not_constructed, type));
      }
    }

    void track_deallocate(void* ptr, std::size_t num, std::size_t size,
                          std::align_val_t align, alloc_type atype, std::string_view type) {
      auto p = std::experimental::make_observer(ptr);
      auto rng = pointers_map_.equal_range({std::experimental::make_observer(ptr), track_type::allocation});
      if (rng.first != rng.second) {
        for (auto it = rng.first; it != rng.second; ++it) {
          auto state = std::get<alloc_state>(it->second);
          if (state == alloc_state::allocated) {
            std::get<alloc_state>(it->second) = alloc_state::deallocated;
            deallocations_++;
            bytes_deallocated_ += num * size;
            break;
          }
          if (std::next(it) == rng.second &&
              (state == alloc_state::deallocated || state == alloc_state::deallocated_not_match)) {
            std::get<alloc_state>(it->second) = alloc_state::already_deallocated;
            deallocations_++;
            bytes_deallocated_ += num * size;
          }
        }
      }
      else {
        pointers_map_.emplace(std::pair{std::experimental::make_observer(ptr), track_type::allocation},
                              std::make_tuple(num_val_t{num}, size, align, atype, alloc_state::not_allocated, type));
      }
    }

    std::optional<alloc_state> state(void* ptr, track_type tt) const {
      auto it = pointers_map_.find(std::pair{std::experimental::make_observer(ptr), tt});
      if (it != std::end(pointers_map_)) {
        return std::make_optional(std::get<alloc_state>(it->second));
      }
      return std::nullopt;
    }

    bool everything_has_been_released() noexcept
    {
      return allocations_ == deallocations_ &&
             bytes_allocated_ == bytes_deallocated_ &&
             constructions_ == destructions_ &&
             std::all_of(std::begin(pointers_map_), std::end(pointers_map_),
                 [](const auto& x) {
                    return std::get<alloc_state>(x.second) == alloc_state::deallocated ||
                            std::get<alloc_state>(x.second) == alloc_state::destroyed;});
    }
  private:
    std::size_t allocations_ = 0;
    std::size_t deallocations_ = 0;
    std::size_t bytes_allocated_ = 0;
    std::size_t bytes_deallocated_ = 0;
    std::size_t constructions_ = 0;
    std::size_t destructions_ = 0;

    alignas(A) buffer pointers_map_buffer_;
    std::pmr::monotonic_buffer_resource pointers_map_resource_
        {pointers_map_buffer_.data(), pointers_map_buffer_.size(), std::pmr::null_memory_resource()};
    container pointers_map_{&pointers_map_resource_};
  };
}
#endif //MEM_TRACKER_TRACKER_HPP
