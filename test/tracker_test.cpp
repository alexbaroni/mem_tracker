#include "catch.hpp"

#include <iterator>
#include "mem_tracker/tracker.hpp"

using namespace mem_tracker;

TEST_CASE("tracking allocation default aligned", "[allocation][default aligned]") {
  SECTION("tracking allocation of an int") {
    tracker<1024> t{};
    REQUIRE(std::begin(t) == std::end(t));
    t.track_allocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                     std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    REQUIRE(std::distance(std::begin(t), std::end(t)) == 1);
    REQUIRE((t.allocations() == 1 && t.bytes_allocated() == sizeof(int)));
    REQUIRE((t.deallocations() == 0 && t.bytes_deallocated() == 0));
  }
}

TEST_CASE("tracking construct default aligned", "[construction][default aligned]") {
  SECTION("tracking construction of an int without allocation") {
    tracker<1024> t{};
    t.track_construct(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                      std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    REQUIRE(std::distance(std::begin(t), std::end(t)) == 1);
    REQUIRE((t.constructions() == 1 && t.destructions() == 0));
  }
  SECTION("tracking construction of an int with allocation") {
    tracker<1024> t{};
    t.track_allocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                     std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_construct(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                      std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    REQUIRE(std::distance(std::begin(t), std::end(t)) == 2);
    REQUIRE((t.allocations() == 1 && t.bytes_allocated() == sizeof(int)));
    REQUIRE((t.deallocations() == 0 && t.bytes_deallocated() == 0));
    REQUIRE((t.constructions() == 1 && t.destructions() == 0));
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::construction) == alloc_state::constructed);
  }
}

TEST_CASE("tracking destroy default aligned", "[destruction][default aligned]") {
  SECTION("tracking destruction of an int not tracked") {
    tracker<1024> t{};
    t.track_destroy(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                    std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    REQUIRE((t.constructions() == 0 && t.destructions() == 0));
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::construction) == alloc_state::not_constructed);
    REQUIRE(!t.everything_has_been_released());
  }
  SECTION("tracking destruction of an int allocated but not constructed") {
    tracker<1024> t{};
    t.track_allocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                     std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_destroy(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                    std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    REQUIRE((t.constructions() == 0 && t.destructions() == 0));
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::construction) == alloc_state::not_constructed);
    REQUIRE(!t.everything_has_been_released());
  }
  SECTION("tracking destruction of an int allocated and constructed") {
    tracker<1024> t{};
    t.track_allocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                     std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_construct(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                      std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_destroy(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                    std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    REQUIRE((t.constructions() == 1 && t.destructions() == 1));
    REQUIRE(std::distance(std::begin(t), std::end(t)) == 2);
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::construction) == alloc_state::destroyed);
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::allocation) == alloc_state::allocated);
    REQUIRE(!t.everything_has_been_released());
  }
  SECTION("tracking destruction of an int allocated already destroyed") {
    tracker<1024> t{};
    t.track_allocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                     std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_construct(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                      std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_destroy(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                    std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_destroy(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                    std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    REQUIRE((t.constructions() == 1 && t.destructions() == 2));
    REQUIRE(std::distance(std::begin(t), std::end(t)) == 2);
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::construction) == alloc_state::already_destroyed);
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::allocation) == alloc_state::allocated);
    REQUIRE(!t.everything_has_been_released());
  }
  SECTION("tracking construction of an int allocated already constructed") {
    tracker<1024> t{};
    t.track_allocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                     std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_construct(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                      std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_construct(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                      std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    REQUIRE((t.constructions() == 2 && t.destructions() == 0));
    REQUIRE(std::distance(std::begin(t), std::end(t)) == 2);
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::construction) == alloc_state::already_constructed);
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::allocation) == alloc_state::allocated);
    REQUIRE(!t.everything_has_been_released());
  }
  SECTION("tracking construction of an int allocated already constructed and destroyed") {
    tracker<1024> t{};
    t.track_allocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                     std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_construct(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                      std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_destroy(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                    std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_construct(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                      std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    REQUIRE((t.constructions() == 2 && t.destructions() == 1));
    REQUIRE(std::distance(std::begin(t), std::end(t)) == 3);
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::construction) == alloc_state::constructed);
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::allocation) == alloc_state::allocated);
    REQUIRE(!t.everything_has_been_released());
  }
}
TEST_CASE("tracking deallocate default aligned", "[destruction][default aligned]") {
  SECTION("tracking destruction of an int allocated") {
    tracker<1024> t{};
    t.track_allocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                     std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_deallocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                       std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    REQUIRE((t.allocations() == 1 && t.deallocations() == 1));
    REQUIRE((t.bytes_allocated() == 4 && t.bytes_deallocated() == 4));
    REQUIRE(std::distance(std::begin(t), std::end(t)) == 1);
    REQUIRE(t.everything_has_been_released());
  }
  SECTION("tracking destruction of an int not allocated") {
    tracker<1024> t{};
    t.track_deallocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                       std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    REQUIRE((t.allocations() == 0 && t.deallocations() == 0));
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::allocation) == alloc_state::not_allocated);
    REQUIRE(!t.everything_has_been_released());
  }
  SECTION("tracking destruction of an int allocated already deallocated") {
    tracker<1024> t{};
    t.track_allocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                     std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_deallocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                       std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_deallocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                       std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    REQUIRE((t.allocations() == 1 && t.deallocations() == 2));
    REQUIRE((t.bytes_allocated() == 4 && t.bytes_deallocated() == 8));
    REQUIRE(std::distance(std::begin(t), std::end(t)) == 1);
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::allocation) == alloc_state::already_deallocated);
    REQUIRE(!t.everything_has_been_released());
  }
  SECTION("tracking allocation of an int allocated already allocated") {
    tracker<1024> t{};
    t.track_allocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                     std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    t.track_allocate(reinterpret_cast<void*>(0x1000000), 1, sizeof(int),
                       std::align_val_t{alignof(int)}, alloc_type::defualt_aligned, "int");
    REQUIRE((t.allocations() == 2 && t.deallocations() == 0));
    REQUIRE((t.bytes_allocated() == 8 && t.bytes_deallocated() == 0));
    REQUIRE(std::distance(std::begin(t), std::end(t)) == 1);
    REQUIRE(*t.state(reinterpret_cast<void*>(0x1000000), track_type::allocation) == alloc_state::already_allocated);
    REQUIRE(!t.everything_has_been_released());
  }
}
