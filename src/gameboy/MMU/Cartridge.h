#pragma once

#include "MBC.h"
#include <cstdint>
#include <vector> //vector
#include <memory> // ptr
#include <fstream>

/**
 * This class emulates a Game Boy cartridge. A ROM-file can be loaded with associated XRAM-file.
 * The XRAM is saved back to file when the application is closed.
 * The cartridge is initialized to use the MBC specified in the ROM-file.
 */
class Cartridge {
public:
    Cartridge();

    /**
     * Reset all variables and initiate rom and mbc to match a ROM_ONLY_MBC.
     */
    void reset();

    /**
     * Read from the rom or ram at a specific address according to the mbc:s read function.
     * @param addr address to read from
     */
    uint8_t read(uint16_t addr) const;

    /**
     * Write to the mbc or ram at the address specified using the mbc:s write function.
     * @param addr address to write to
     * @param data data to write
     */
    void write(uint16_t addr, uint8_t data);

    /**
     * Write directly to the rom at the specified address.
     * To be used only in testing.
     * @param addr address to write to
     * @param data data to write
     */
    void writeTest(uint16_t addr, uint8_t data);

    /**
     * Load a rom file into memory.
     * @param filepath filepath to load rom-file from
     * @param load_ram_from_file whether to load corresponding ram file or not
     * @return false, if unable to open file, or if romSize, ramSize or cartridgeType is unsupported
     * @return true, if file is loaded successfully
     */
    bool loadRom(const std::string& filepath, bool load_ram_from_file=false);

    /**
     * Save the contents of the ram, if any, to a file.
     * @return false, if unable to open file or unable to write to file
     * @return true, write to file successful
     */
    bool saveRam();

    /**
     * Load a ram file to memory.
     * @returns false, if unable to open file, or if file size does not match ramSize
     * @returns true, if load successful
     */
    bool loadRam();

    /**
     * Update the mbc, which in turn updates rtc (real time clock), if any.
     * @param cycles the amount of cycles to update
     */
    void update(uint8_t cycles);

private:
    enum CartridgeType {
        // When adding cartridge support add it to Cartridge::init_mbc()
        ROM_ONLY = 0x0,
        MBC1 = 0x1,
        MBC1_R = 0x2,
        MBC1_R_B = 0x3,
        MBC3_T_B = 0xf,
        MBC3_T_R_B = 0x10,
        MBC3 = 0x11,
        MBC_R = 0x12,
        MBC_R_B = 0x13,
    };

    enum ROM_Size {
        ROM_32KB = 0x0,
        ROM_64KB = 0x1,
        ROM_128KB = 0x2,
        ROM_256KB = 0x3,
        ROM_512KB = 0x4,
        ROM_1MB = 0x5,
        ROM_2MB = 0x6,
        ROM_4MB = 0x7,
        ROM_8MB = 0x8,
    };

    enum RAM_Size {
        RAM_NO_RAM = 0x0,
        RAM_8KB = 0x2,
        RAM_32KB = 0x3,
        RAM_128KB = 0x4,
        RAM_64KB = 0x5,
    };

    uint8_t cartridgeType;
    uint8_t romSize;
    uint8_t ramSize;
    std::vector<uint8_t> rom;
    std::vector<uint8_t> ram;
    std::unique_ptr<MBC> mbc;
    std::string filepath;

    /**
     * Initiate rom to the size according to romSize.
     * @return false, if unsupported romSize.
     * @return true, if size successfully initiated
     */
    bool initRom();

    /**
     * Initiate ram to the size according to ramSize.
     * If loadFromRam is true, load ram from file.
     * @param loadFromRam whether to load the ram from a file
     * @return false, if unsupported romSize.
     * @return true, if size successfully initiated
     */
    bool initRam(bool loadFromRam=false);

    /**
     * Initiates mbc with an mbc instance according to cartridgeType.
     * @return false, if unsupported cartridgeType
     * @return true, if successful initiation
     */
    bool initMbc();
};
