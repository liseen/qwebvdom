TEMPLATE = app
SOURCES +=\
    test.cpp 

HEADERS += qwebvdom.h

INCLUDEPATH  = ./ ../libvdom/ 
LIBS = -lprotobuf -lvdom 
QT += webkit
