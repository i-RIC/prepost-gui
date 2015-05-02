LIBS += -L/usr/local/lib
LIBS += -L/usr/local/qwt-6.1.2/lib
LIBS += -L/usr/local/iRIC/lib

INCLUDEPATH += .

# gdal
INCLUDEPATH += /usr/include/gdal

# vtk
INCLUDEPATH += /usr/local/include/vtk-6.1

# iriclib
INCLUDEPATH += /usr/local/iRIC/include

# Qwt
INCLUDEPATH += /usr/local/qwt-6.1.2/include

#internal libs
INCLUDEPATH += /home/keisuke/iric_dev/src/libs

CONFIG += c++11

target.path = /usr/local/iRIC/lib
INSTALLS += target

