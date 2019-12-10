#ifndef MEM_TRACKER_TRACK_TYPE_HPP
#define MEM_TRACKER_TRACK_TYPE_HPP

namespace mem_tracker {
  enum class track_type { allocation, construction };

  struct track_type_hash
  {
    std::size_t operator()(track_type const& t) const noexcept
    {
      return static_cast<std::size_t>(t);
    }
  };
}

#endif //MEM_TRACKER_TRACK_TYPE_HPP
