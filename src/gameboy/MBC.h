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
};

class ROM_Only_MBC : public MBC {
public:
    explicit ROM_Only_MBC(std::shared_ptr<std::vector<uint8_t>> rom);
    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;

private:
    std::shared_ptr<std::vector<uint8_t>> rom;
};

class MBC1_MBC : public MBC {
public:
    MBC1_MBC(std::shared_ptr<std::vector<uint8_t>> rom, std::shared_ptr<std::vector<uint8_t>> ram);
    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;

private:
    uint8_t ram_enable;
    uint8_t rom_bank_number;
    uint8_t ram_bank_number;
    uint8_t banking_mode;

    std::shared_ptr<std::vector<uint8_t>> rom;
    std::shared_ptr<std::vector<uint8_t>> ram;

    static uint16_t rom_bank_mask(uint32_t size);
    static uint16_t ram_bank_mask(uint32_t size);
};
