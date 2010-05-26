TEMPLATE = app
SOURCES +=\
    test.cpp 

HEADERS += vdom.h qwebvdom.h

INCLUDEPATH  = ./
LIBS = -lprotobuf -lvdom 
QT += webkit
