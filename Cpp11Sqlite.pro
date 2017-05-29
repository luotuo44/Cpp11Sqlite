TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    SqliteDB.cpp \
    PreparedStatement.cpp \
    QueryResultSet.cpp

HEADERS += \
    SqliteDB.h \
    PreparedStatement.h \
    QueryResultSet.h


INCLUDEPATH += sqlite3



#LIBS += sqlite3/libsqlite3.a  #如果是在项目目录构建也可以使用本条配置

LIBS += -L"$$PWD/sqlite3" -lsqlite3




QMAKE_CXXFLAGS += -std=c++11

DISTFILES += \
    README.md
