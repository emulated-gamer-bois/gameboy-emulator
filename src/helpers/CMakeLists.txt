cmake_minimum_required ( VERSION 3.0.2 )
project( helpers )

add_library( ${PROJECT_NAME}
        AppTimer.cpp
        AppTimer.h
        ErrorReport.h
        )