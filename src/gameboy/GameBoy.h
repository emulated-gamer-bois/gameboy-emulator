
#pragma once

#include <memory> //ptr
#include "CPU/CPU.h"
#include "MMU/MMU.h"
#include "PPU/PPU.h"
#include "Joypad.h"
#include "MMU/Timer.h"
#include "MMU/Cartridge.h"
#include "APU/IVolumeController.h"
#include "APU/APUState.h"


#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test

#define JOYPAD_RELEASE 0
#define JOYPAD_PRESS   1

class GameBoy {
public:
    GameBoy();
    /**
     * Steps the emulation the equivalent machine cycles of one CPU-instruction.
     * All other units are synchronized to the execution of the CPU-instructions.
     * @param vc is used to alter volume.
     * */
    void step(IVolumeController* vc);
    //TODO Comment exactly how this works and why?
    std::unique_ptr<uint8_t[]> getScreenTexture();
    /**
     * Handles the joypad input and redirects it to the Joypad to handle.
     * @param key which key has changed.
     * @param action if the action was press or release.
     * */
    void joypadInput(uint8_t key, uint8_t action);
    /**
     * Resets the emulation and loads new boot and game ROMs.
     * If no or an invalid boot ROM is provided, the boot phase is skipped.
     * @param bootFilepath path to boot ROM.
     * @param romFilepath path to game ROM.
     * */
    void loadRom(std::string bootFilepath, std::string romFilepath);
    /**
     * Loads game ROM.
     * @param path to game ROM.
     * */
    void loadGameRom(std::string filepath);
    /**
    * Loads boot ROM.
    * @param path to boot ROM.
    * */
    void loadBootRom(std::string filepath);
    /**
     * Returns whether or not the PPU has completed a whole frame and is ready to produce this.
     * */
    bool isReadyToDraw() const;
    /**
     * Resets the status of PPU, confirming that the recently produced frame has been handled.
     * */
    void confirmDraw();
    /**
     * Prints all information about CPU state. Useful for debugging.
     * */
    void cpuDump();
    /**
     * Returns a bool depending on if the emulator is turned on or off.
     * */
    bool isOn() const;
    /**
     * Saves RAM-data to a file, this used in games which have XRAM which is battery powered, and hence supplies
     *  an option for the player to save game progress.
     *  @return returns whether or not the save was a success.
    * */
    bool save();

  /**
   * Indicates if the state of the APU has changed, meaning the audio output should change
   * @return bit x == 1 -> source x has changed and the output should change
   */
    uint8_t isReadyToPlaySound();

    /**
     * Resets the value returned from isReadyToPlaySound
     */
    void confirmPlay();

    /**
     * Returns the relevant information audio all audio channels
     * Make sure to delete the state afterwards in order to prevent memory leaks
     * @return the state of the APU
     */
    APUState* getAPUState();

private:
    bool on;

    std::shared_ptr<MMU> mmu;
    std::unique_ptr<CPU> cpu;
    std::shared_ptr<PPU> ppu;
    std::shared_ptr<APU> apu;

    std::shared_ptr<Joypad> joypad;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<Cartridge> cartridge;

    FRIEND_TEST(PPU, g_tile_rom);
};
