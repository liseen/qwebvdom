TEMPLATE = app
SOURCES +=\
    test.cpp 

HEADERS += qwebvdom.h

INCLUDEPATH  = /opt/qcrawler-thirdparty/include
LIBS = -L /opt/qcrawler-thirdparty/lib -lprotobuf -lvdom 
QT += webkit
