//
// Created by isaaklindgren on 2021-03-19.
//

#include <iostream>
#include "Keybinds.h"


void Keybinds::init_keybinds() {
    a.keyval = 102;
    b.keyval = 103;
    start.keyval= 104;
    select.keyval = 106;
    left.keyval = 97;
    right.keyval = 100;
    up.keyval = 119;
    down.keyval = 115;
    a.keybind = 'f';
    b.keybind = 'g';
    start.keybind = 'h';
    select.keybind = 'j';
    left.keybind = 'a';
    right.keybind = 'd';
    up.keybind = 'w';
    down.keybind = 's';
    a.action_description = "A";
    b.action_description = "B";
    start.action_description = "Start";
    select.action_description = "Select";
    left.action_description = "Gamepad Left";
    right.action_description = "Gamepad Right";
    up.action_description = "Gamepad Up";
    down.action_description = "Gamepad Down";
    start.action_description = "Start";
    nControllers = 8;
}