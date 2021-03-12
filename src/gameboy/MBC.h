//
// Created by Algot on 2021-03-11.
//

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <iostream>

class MBC {
public:
    virtual uint8_t read(uint16_t addr) = 0;
    virtual void write(uint16_t addr, uint8_t data) = 0;
};

class ROM_Only_MBC : public MBC {
public:
    ROM_Only_MBC(std::shared_ptr<std::vector<uint8_t>> rom);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);

private:
    std::shared_ptr<std::vector<uint8_t>> rom;
};

class MBC1_MBC : public MBC {
public:
    MBC1_MBC(std::shared_ptr<std::vector<uint8_t>> rom, std::shared_ptr<std::vector<uint8_t>> ram);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);

private:
    uint8_t ram_enable;
    uint8_t rom_bank_number;
    uint8_t ram_bank_number;
    uint8_t banking_mode;

    std::shared_ptr<std::vector<uint8_t>> rom;
    std::shared_ptr<std::vector<uint8_t>> ram;
};
