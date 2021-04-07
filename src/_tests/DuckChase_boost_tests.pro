TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += BOOST_TEST

QMAKE_CXXFLAGS += -Og -g -std=c++11 -Wall -Wconversion -Wextra -pedantic #-Weffc++

LIBS += -lboost_unit_test_framework \

INCLUDEPATH +=  . \
                ../anim \
                ../shapes

SOURCES += \
    DuckChase_boost_tests.cpp \
    ../shapes/Rect.cpp

HEADERS += \
    amiga_types.h \
    ../shapes/Rect.h
