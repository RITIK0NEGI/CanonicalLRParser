QT += widgets
CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    GrammarInput.cpp \
    AugmentedGrammar.cpp \
    FirstFollow.cpp \
    ItemSetGenerator.cpp \
    CanonicalLRParser.cpp

HEADERS += \
    mainwindow.h \
    GrammarInput.h \
    AugmentedGrammar.h \
    FirstFollow.h \
    ItemSetGenerator.h \
    CanonicalLRParser.h

# No FORMS section since we're not using .ui files
