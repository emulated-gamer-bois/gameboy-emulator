//
// Created by algot on 2021-02-15.
//

#include <memory>

#include "gtest/gtest.h"
#include "../src/gameboy/mmu.h"

TEST(MMU, read_write){
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    mmu->disable_boot_rom();

    mmu->write(0x30, 0xa1);
    ASSERT_EQ(mmu->read(0x30), 0xa1);

    mmu->write(0x30, 0xd5);
    ASSERT_EQ(mmu->read(0x30), 0xd5);
}

//TEST(MMU, load_rom){
//    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
//    mmu->disable_boot_rom();
//    mmu->load_rom("rom.gb");
//
//    ASSERT_EQ(mmu->read(0x104), 0xce);
//    ASSERT_EQ(mmu->read(0x105), 0xed);
//}
