cmake_minimum_required ( VERSION 3.0.2 )

project ( application )

add_library( ${PROJECT_NAME}
        Application.cpp
        AppSettings.h
        AppSettings.cpp
        AppSettings.h
        KeyBinds.cpp
        KeyBinds.h
        Game-Boy-FL.cpp
        )

target_link_libraries ( ${PROJECT_NAME} IO helpers gameboy )

