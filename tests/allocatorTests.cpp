#include "catch.hpp"
#include "slabAllocator.h"
#include "codegen.h"
#include <iostream>

using babycpp::memory::Slab;
using babycpp::memory::SlabAllocator;

TEST_CASE("Testing allocator not movable", "[memory]") {
  REQUIRE(std::is_copy_constructible<SlabAllocator>::value == false);
  REQUIRE(std::is_copy_assignable<SlabAllocator>::value == false);
}

TEST_CASE("Testing allocator instantiation", "[memory]") {
  SlabAllocator slab;
  REQUIRE(slab.currentSlab != nullptr);

  //making sure there is at least one slab when istantiated
  Slab &currSlab = *slab.currentSlab;
  REQUIRE(slab.currentSlab != nullptr);
  REQUIRE(slab.currentSlab == &slab.slabs[0]);
  REQUIRE((currSlab.endp - currSlab.data) == babycpp::memory::SLAB_SIZE);
}
TEST_CASE("Testing allocator instantiation, not default slab size", "[memory]") {
  SlabAllocator slab(200);
  REQUIRE(slab.currentSlab != nullptr);

  //making sure there is at least one slab when istantiated
  Slab &currSlab = *slab.currentSlab;
  REQUIRE(slab.currentSlab != nullptr);
  REQUIRE(slab.currentSlab == &slab.slabs[0]);
  REQUIRE((currSlab.endp - currSlab.data) == 200);
}
TEST_CASE("Testing allocation of memory", "[memory]") {
  SlabAllocator slab;

  uint32_t allocSize = sizeof(babycpp::codegen::ExprAST);
  slab.alloc( allocSize);

  uint64_t offset = slab.getStackPtrOffset();
  REQUIRE(offset  == allocSize);
}

TEST_CASE("Testing allocation loop alloc and free", "[memory]") {
  SlabAllocator slab;

  uint32_t allocSize = sizeof(babycpp::codegen::ExprAST);

  for(uint32_t i=0; i <20; ++i )
  {
    auto* p= slab.alloc( allocSize);
    REQUIRE(p != nullptr);
  }
  uint64_t offset = slab.getStackPtrOffset();
  REQUIRE(offset  == allocSize*20);
}

TEST_CASE("Testing allocation slab-re-alloc", "[memory]") {

  uint32_t allocSize = sizeof(babycpp::codegen::ExprAST);
  SlabAllocator slab(allocSize*30);

  for(uint32_t i=0; i <30; ++i )
  {
    auto* p= slab.alloc( allocSize);
    REQUIRE(p != nullptr);
  }
  REQUIRE(slab.slabs.size()==1);

  //now we are exactly on the edge of the allocation, new allocation
  //should trigger a new slab alloc
  Slab* curr = slab.currentSlab;
  auto* p= slab.alloc( allocSize);
  REQUIRE(p != nullptr);
  REQUIRE(curr != slab.currentSlab);
  REQUIRE(slab.slabs.size()==2);
  //since there should be only one element in the new slab alloc
  //the offset should be the size of one alloc
  REQUIRE(slab.getStackPtrOffset()==allocSize);

}

TEST_CASE("Testing ", "[memory]") {

  uint32_t allocSize = sizeof(babycpp::codegen::ExprAST);
  SlabAllocator slab(allocSize*9);

  for(uint32_t i=0; i <30; ++i )
  {
    auto* p= slab.alloc( allocSize);
    REQUIRE(p != nullptr);
  }
  REQUIRE(slab.slabs.size()==4);
  slab.clear();
  REQUIRE(slab.slabs.size()==1);
}