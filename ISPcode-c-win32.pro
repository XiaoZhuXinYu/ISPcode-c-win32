TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        bmpimage.c \
        cj_2dnr.c \
        cj_base_alg.c \
        cj_debug.c \
        main.c

HEADERS += \
    bmpimage.h \
    cj_2dnr.h \
    cj_base_alg.h \
    cj_debug.h \
    include/bmpimage.h \
    include/cj_2dnr.h \
    include/cj_base_alg.h \
    include/cj_debug.h
