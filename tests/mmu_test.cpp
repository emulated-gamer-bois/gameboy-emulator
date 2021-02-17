//
// Created by algot on 2021-02-15.
//

#include <memory>

#include "gtest/gtest.h"
#include "../src/gameboy/mmu.h"

TEST(MMU, read_write){
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    mmu->disable_boot_rom();

    mmu->write_ONLY_IN_TESTS(0x30, 0xa1);
    ASSERT_EQ(mmu->read(0x30), 0xa1);

    mmu->write_ONLY_IN_TESTS(0x30, 0xd5);
    ASSERT_EQ(mmu->read(0x30), 0xd5);
}

TEST(MMU, read_write_interrupt_enable){
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    mmu->disable_boot_rom();

    // Test default value
    ASSERT_EQ(mmu->read(0xffff), 0x1f);

    mmu->write(0xffff, 0x0a);
    ASSERT_EQ(mmu->read(0xffff), 0x0a);
}

//TEST(MMU, load_rom){
//    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
//    mmu->disable_boot_rom();
//    mmu->load_rom("rom.gb");
//
//    ASSERT_EQ(mmu->read(0x104), 0xce);
//    ASSERT_EQ(mmu->read(0x105), 0xed);
//}
