#ifndef MEM_TRACKER_ALLOC_STATE_HPP
#define MEM_TRACKER_ALLOC_STATE_HPP

#include <string>
#include <unordered_map>

namespace mem_tracker {
  enum class alloc_state {
    allocated, deallocated, not_allocated, already_allocated, already_deallocated, deallocated_not_match,
    constructed, destroyed, not_constructed, already_constructed, already_destroyed, destroyed_not_match
  };

  struct alloc_state_decoder {
    std::string const& operator()(alloc_state as) const {
      return alloc_state_decoder::map_[as];
    }

    static inline std::unordered_map<alloc_state, std::string> map_ {
      {alloc_state::allocated, "allocated"},
      {alloc_state::deallocated, "deallocated"},
      {alloc_state::not_allocated, "not allocated"},
      {alloc_state::already_allocated, "already allocated"},
      {alloc_state::already_deallocated, "already deallocated"},
      {alloc_state::deallocated_not_match, "deallocated not match"},
      {alloc_state::constructed, "constructed"},
      {alloc_state::destroyed, "destroyed"},
      {alloc_state::not_constructed, "not constructed"},
      {alloc_state::already_constructed, "already constructed"},
      {alloc_state::already_destroyed, "already destroyed"},
      {alloc_state::destroyed_not_match, "destroyed not match"}
    };
  };
}
#endif //MEM_TRACKER_ALLOC_STATE_HPP
