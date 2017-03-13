TEMPLATE = app
TARGET = Rivmaker
CONFIG += qt
CONFIG += debug_and_release

include( ../../paths.pri )

QT += widgets network
RC_FILE = rivmaker.rc

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkInteractionStyle-6.1 \
	-lvtkIOExport-6.1 \
	-lvtkRenderingFreeType-6.1 \
	-lvtkRenderingFreeTypeOpenGL-6.1 \
	-lvtkRenderingOpenGL-6.1

# Input
HEADERS += data/baseline.h \
           data/crosssection.h \
           data/elevationpoints.h \
           data/project.h \
           data/riversurveydata.h \
           data/riversurveydatacreator.h \
           data/riversurveydatacrosssection.h \
           data/watersurfaceelevationpoints.h \
           dialogs/mousehelpdialog.h \
           main/rivmakermainwindow.h \
           data/private/baseline_impl.h \
           data/private/project_impl.h \
           main/private/rivmakermainwindow_impl.h \
           window/crosssection/crosssectionwindow.h \
           window/preprocessor/preprocessormodel.h \
           window/preprocessor/preprocessorview.h \
           window/preprocessor/preprocessorwindow.h \
           window/verticalcrosssection/verticalcrosssectionwindow.h \
           window/preprocessor/objectbrowser/objectbrowser.h \
           window/preprocessor/objectbrowser/objectbrowserview.h \
           window/preprocessor/private/preprocessormodel_impl.h \
           window/preprocessor/private/preprocessorwindow_impl.h
FORMS += dialogs/mousehelpdialog.ui \
         main/rivmakermainwindow.ui \
         window/verticalcrosssection/verticalcrosssectionwindow.ui
SOURCES += data/baseline.cpp \
           data/crosssection.cpp \
           data/elevationpoints.cpp \
           data/project.cpp \
           data/riversurveydata.cpp \
           data/riversurveydatacreator.cpp \
           data/riversurveydatacrosssection.cpp \
           data/watersurfaceelevationpoints.cpp \
           dialogs/mousehelpdialog.cpp \
           main/main.cpp \
           main/rivmakermainwindow.cpp \
           main/rivmakermainwindow_setupconnections.cpp \
           window/crosssection/crosssectionwindow.cpp \
           window/preprocessor/preprocessormodel.cpp \
           window/preprocessor/preprocessorview.cpp \
           window/preprocessor/preprocessorwindow.cpp \
           window/verticalcrosssection/verticalcrosssectionwindow.cpp \
           window/preprocessor/objectbrowser/objectbrowser.cpp \
           window/preprocessor/objectbrowser/objectbrowserview.cpp
RESOURCES += rivmaker.qrc
