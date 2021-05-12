#pragma once

#include <vector> //vector
#include <cstdint>

/**
 * The MBC class is an interface to be used when implementing different MBCs.
 * All MBCs have common needs for a read and write function but also an update
 * function that can be used to update devices on the MBC.
 */
class MBC {
public:
    /**
     * Read a byte from the specified address.
     * Different behaviour specific for different MBCs.
     * @param addr address to read from
     * @returns data stored on address
     */
    virtual uint8_t read(uint16_t addr) = 0;

    /**
     * Write a byte to the specified address.
     * Different behaviour specific for different MBCs.
     * Used to access control registers, or to access XRAM.
     * @param addr address to write to
     * @param data data to be written to address
     */
    virtual void write(uint16_t addr, uint8_t data) = 0;

    /**
     * Update the MBC to be synced with the system.
     * For example used to update RTC, which exist with some MBCs.
     * @param cycles the amount of cycles to proceed.
     */
    virtual void update(uint8_t cycles) = 0;

    /**
     * Returns a bitmask that, when applied, truncate a memory bank number
     * to prevent accessing memory larger than allocated (index out of bounds).
     * @param size ROM size
     */
    static uint16_t romBankMask(uint32_t size);

    /**
     * Returns a bitmask that, when applied, truncate a memory bank number
     * to prevent accessing memory larger than allocated (index out of bounds).
     * @param size RAM size
     */
    static uint16_t ramBankMask(uint32_t size);
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
    uint8_t ramEnable;
    uint8_t romBankNumber;
    uint8_t ramBankNumber;
    uint8_t bankingMode;

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
    void rtcLatch();
    uint8_t rtcRegister;
    uint8_t ramTimerEnable;
    uint8_t romBankNumber;
    uint8_t ramBankNumberRtcRegisterSelect;
    uint8_t latchClockData;

    uint32_t rtcSubseconds;
    uint8_t rtcHalt;
    uint8_t rtcSeconds;
    uint8_t rtcMinutes;
    uint8_t rtcHours;
    uint16_t rtcDays;
    uint8_t rtcDaysOverflow;

    uint8_t rtcHaltLatched;
    uint8_t rtcSecondsLatched;
    uint8_t rtcMinutesLatched;
    uint8_t rtcHoursLatched;
    uint16_t rtcDaysLatched;
    uint8_t rtcDaysOverflowLatched;

    std::vector<uint8_t> *rom;
    std::vector<uint8_t> *ram;
};
