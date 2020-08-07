QT += testlib
#QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../App/
#include(../App/GEOINTMonitor.pri)

SOURCES +=  tst_gdelttestsuite.cpp
