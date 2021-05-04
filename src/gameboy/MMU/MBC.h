#pragma once

#include <vector> //vector
#include <iostream> //cout

class MBC {
public:
    virtual uint8_t read(uint16_t addr) = 0;
    virtual void write(uint16_t addr, uint8_t data) = 0;
    virtual void update(uint8_t cycles) = 0;
    static uint16_t romBankMask(uint32_t size);
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
