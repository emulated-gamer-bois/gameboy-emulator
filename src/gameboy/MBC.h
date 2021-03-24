//
// Created by Algot on 2021-03-11.
//

#pragma once

#include <cstdint>
#include <vector>
#include <memory>

class MBC {
public:
    virtual uint8_t read(uint16_t addr) = 0;
    virtual void write(uint16_t addr, uint8_t data) = 0;
    virtual void update(uint8_t cycles) = 0;
    static uint16_t rom_bank_mask(uint32_t size);
    static uint16_t ram_bank_mask(uint32_t size);
};

class ROM_Only_MBC : public MBC {
public:
    explicit ROM_Only_MBC(std::vector<uint8_t> *rom);

    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;
    void update(uint8_t cycles) override {}

private:
    std::vector<uint8_t> *rom;
};

class MBC1_MBC : public MBC {
public:
    MBC1_MBC(std::vector<uint8_t> *rom, std::vector<uint8_t> *ram);

    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;

    void update(uint8_t cycles) override {}

private:
    uint8_t ram_enable;
    uint8_t rom_bank_number;
    uint8_t ram_bank_number;
    uint8_t banking_mode;

    std::vector<uint8_t> *rom;
    std::vector<uint8_t> *ram;
};

class MBC3_MBC : public MBC {
public:
    MBC3_MBC(std::vector<uint8_t> *rom, std::vector<uint8_t> *ram);
    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;
    void update(uint8_t cycles) override;

private:
    void rtc_latch();
    uint8_t rtc_register;
    uint8_t ram_timer_enable;
    uint8_t rom_bank_number;
    uint8_t ram_bank_number_rtc_register_select;
    uint8_t latch_clock_data;

    uint32_t rtc_subseconds;
    uint8_t rtc_halt;
    uint8_t rtc_seconds;
    uint8_t rtc_minutes;
    uint8_t rtc_hours;
    uint16_t rtc_days;
    uint8_t rtc_days_overflow;

    uint8_t rtc_halt_latched;
    uint8_t rtc_seconds_latched;
    uint8_t rtc_minutes_latched;
    uint8_t rtc_hours_latched;
    uint16_t rtc_days_latched;
    uint8_t rtc_days_overflow_latched;

    std::vector<uint8_t> *rom;
    std::vector<uint8_t> *ram;
};
