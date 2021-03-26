//
// Created by algot on 2021-02-15.
//

#include <memory>

#include "gtest/gtest.h"
#include "../src/gameboy/MMU.h"
#include "../src/gameboy/Joypad.h"
#include "../src/gameboy/Timer.h"

TEST(MMU, read_write){
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::shared_ptr<Cartridge> cartridge = std::make_shared<Cartridge>();
    mmu->link_devices(nullptr, nullptr, nullptr, cartridge);

    // Disable boot ROM
    mmu->write(0xff50, 0x01);

    mmu->write_GAME_ROM_ONLY_IN_TESTS(0x30, 0xa1);
    ASSERT_EQ(mmu->read(0x30), 0xa1);

    mmu->write(0xc030, 0xd5);
    ASSERT_EQ(mmu->read(0xc030), 0xd5);
}

TEST(MMU, read_write_interrupt_enable){
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();

    // Disable boot ROM
    mmu->write(0xff50, 0x01);

    // Test default value
    ASSERT_EQ(mmu->read(0xffff), 0x1f);

    mmu->write(0xffff, 0x0a);
    ASSERT_EQ(mmu->read(0xffff), 0x0a);
}

TEST(MMU, disable_boot_rom){
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::shared_ptr<Cartridge> cartridge = std::make_shared<Cartridge>();
    mmu->link_devices(nullptr, nullptr, nullptr, cartridge);

    mmu->write_BOOT_ROM_ONLY_IN_TESTS(0x00, 0x55);
    ASSERT_EQ(mmu->read(0x00), 0x55);

    // Disable boot ROM
    mmu->write(0xff50, 0x01);

    ASSERT_EQ(mmu->read(0x00), 0x00);
}

TEST(MMU, joypad){
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::shared_ptr<Joypad> joypad = std::make_shared<Joypad>(mmu);
    mmu->link_devices(nullptr, joypad, nullptr, nullptr);

    mmu->write(IO_JOYPAD, JOYPAD_SEL_BUTTONS);
    ASSERT_EQ(mmu->read(IO_JOYPAD), 0b1111);
    mmu->write(IO_JOYPAD, JOYPAD_SEL_DIRECTIONS);
    ASSERT_EQ(mmu->read(IO_JOYPAD), 0b1111);

    // Press Left button
    joypad->press(JOYPAD_LEFT);
    mmu->write(IO_JOYPAD, JOYPAD_SEL_DIRECTIONS);
    ASSERT_EQ(mmu->read(IO_JOYPAD), 0b1101);

    // Press Down button
    joypad->press(JOYPAD_DOWN);
    mmu->write(IO_JOYPAD, JOYPAD_SEL_DIRECTIONS);
    ASSERT_EQ(mmu->read(IO_JOYPAD), 0b0101);

    // Press Select button
    joypad->press(JOYPAD_SELECT);
    mmu->write(IO_JOYPAD, JOYPAD_SEL_DIRECTIONS);
    ASSERT_EQ(mmu->read(IO_JOYPAD), 0b0101);

    mmu->write(IO_JOYPAD, JOYPAD_SEL_BUTTONS);
    ASSERT_EQ(mmu->read(IO_JOYPAD), 0b1011);

    // Release Down button
    joypad->release(JOYPAD_DOWN);

    mmu->write(IO_JOYPAD, JOYPAD_SEL_BUTTONS);
    ASSERT_EQ(mmu->read(IO_JOYPAD), 0b1011);

    mmu->write(IO_JOYPAD, JOYPAD_SEL_DIRECTIONS);
    ASSERT_EQ(mmu->read(IO_JOYPAD), 0b1101);
}

TEST(MMU, timer){
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::shared_ptr<Timer> timer = std::make_shared<Timer>(mmu);
    mmu->link_devices(nullptr, nullptr, timer, nullptr);

    // Disable boot ROM
    mmu->write(0xff50, 0x01);

    // Activate and set timer speed to 65,536Hz
    // (Speed: 65,536Hz require 4 1MHz cycles to increase counter)
    mmu->write(0xff07, 0b101);
    ASSERT_EQ(mmu->read(0xff07), 0b101);
    ASSERT_EQ(mmu->read(0xff05), 0);

    // Should not increase counter
    timer->update(3);
    ASSERT_EQ(mmu->read(0xff05), 0);

    // Reset divider
    mmu->write(0xff04, 123);

    // Should still not increase counter due to divider reset
    timer->update(3);
    ASSERT_EQ(mmu->read(0xff05), 0);

    // Should increase counter
    timer->update(1);
    ASSERT_EQ(mmu->read(0xff05), 1);

    // Should increase counter to 0xff
    timer->update(4*(0xff)-1);
    ASSERT_EQ(mmu->read(0xff05), 0xff);

    // Set modulo to 0x55
    mmu->write(0xff06, 0x55);

    // Should raise interrupt request flag
    // Counter should be 0x55
    timer->update(4);
    ASSERT_EQ(mmu->read(0xff0f), (1 << 2));
    ASSERT_EQ(mmu->read(0xff05), 0x55);
}
