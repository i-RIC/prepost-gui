TARGET = iricGdPolydataGroup
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_POLYDATAGROUP_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

# iricGdPolydata

win32 {
        CONFIG(debug, debug|release) {
                LIBS += -L"../polydata/debug"
        } else {
                LIBS += -L"../polydata/release"
        }
}
unix {
        LIBS += -L"../polydata"
}
LIBS += -liricGdPolydata

# iricGuibase

win32 {
        CONFIG(debug, debug|release) {
                LIBS += -L"../../guibase/debug"
        } else {
                LIBS += -L"../../guibase/release"
        }
}
unix {
        LIBS += -L"../../guibase"
}
LIBS += -liricGuibase

# iricGuicore

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../guicore/debug"
	} else {
		LIBS += -L"../../guicore/release"
	}
}
unix {
	LIBS += -L"../../guicore"
}
LIBS += -liricGuicore

# iricMisc

win32 {
        CONFIG(debug, debug|release) {
                LIBS += -L"../../misc/debug"
        } else {
                LIBS += -L"../../misc/release"
        }
}
unix {
        LIBS += -L"../../misc"
}
LIBS += -liricMisc

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkRenderingCore-6.1

# shapelib

win32{
LIBS += -lshapelib_i
}

# geos

LIBS += -lgeos

# Post-Build Event
win32 {
	DESTDIR = $(SolutionDir)\\libdlls\\$(Configuration)
	LIBS += -L$(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += gd_polydatagroup_global.h \
           geodatapolydatagroup.h \
           geodatapolydatagroupattributebrowser.h \
           geodatapolydatagroupcolorsettingdialog.h \
           geodatapolydatagroupcreator.h \
           geodatapolydatagroupmergesettingdialog.h \
           geodatapolydatagrouppolydata.h \
           geodatapolydatagrouppolydatawithboundingrect.h \
           geodatapolydatagroupshpimporter.h \
           geodatapolydatagroupshpimportersettingdialog.h \
           private/geodatapolydatagroup_editnameandvaluecommand.h \
           private/geodatapolydatagroup_impl.h \
           private/geodatapolydatagroup_setcolorsettingcommand.h \
           private/geodatapolydatagroup_sortcommand.h \
           private/geodatapolydatagroup_sortedittargetdatacommand.h \
           private/geodatapolydatagroupattributebrowser_tableitemdelegate.h \
           private/geodatapolydatagrouppolydata_impl.h
FORMS += geodatapolydatagroupcolorsettingdialog.ui \
         geodatapolydatagroupmergesettingdialog.ui \
         geodatapolydatagroupshpimportersettingdialog.ui
SOURCES += geodatapolydatagroup.cpp \
           geodatapolydatagroupattributebrowser.cpp \
           geodatapolydatagroupcolorsettingdialog.cpp \
           geodatapolydatagroupcreator.cpp \
           geodatapolydatagroupmergesettingdialog.cpp \
           geodatapolydatagrouppolydata.cpp \
           geodatapolydatagrouppolydatawithboundingrect.cpp \
           geodatapolydatagroupshpimporter.cpp \
           geodatapolydatagroupshpimportersettingdialog.cpp \
           private/geodatapolydatagroup_editnameandvaluecommand.cpp \
           private/geodatapolydatagroup_impl.cpp \
           private/geodatapolydatagroup_setcolorsettingcommand.cpp \
           private/geodatapolydatagroup_sortcommand.cpp \
           private/geodatapolydatagroup_sortedittargetdatacommand.cpp \
           private/geodatapolydatagroupattributebrowser_tableitemdelegate.cpp
TRANSLATIONS += languages/iricGdPolydataGroup_ar_EG.ts \
                languages/iricGdPolydataGroup_bg_BG.ts \
                languages/iricGdPolydataGroup_bs_BA.ts \
                languages/iricGdPolydataGroup_ca_ES.ts \
                languages/iricGdPolydataGroup_cs_CZ.ts \
                languages/iricGdPolydataGroup_da_DK.ts \
                languages/iricGdPolydataGroup_de_DE.ts \
                languages/iricGdPolydataGroup_el_GR.ts \
                languages/iricGdPolydataGroup_es_ES.ts \
                languages/iricGdPolydataGroup_et_EE.ts \
                languages/iricGdPolydataGroup_eu_ES.ts \
                languages/iricGdPolydataGroup_fi_FI.ts \
                languages/iricGdPolydataGroup_fr_FR.ts \
                languages/iricGdPolydataGroup_gl_ES.ts \
                languages/iricGdPolydataGroup_hi_IN.ts \
                languages/iricGdPolydataGroup_hu_HU.ts \
                languages/iricGdPolydataGroup_id_ID.ts \
                languages/iricGdPolydataGroup_is_IS.ts \
                languages/iricGdPolydataGroup_it_IT.ts \
                languages/iricGdPolydataGroup_ja_JP.ts \
                languages/iricGdPolydataGroup_ko_KR.ts \
                languages/iricGdPolydataGroup_ky_KG.ts \
                languages/iricGdPolydataGroup_lt_LT.ts \
                languages/iricGdPolydataGroup_lv_LV.ts \
                languages/iricGdPolydataGroup_nb_NO.ts \
                languages/iricGdPolydataGroup_nl_NL.ts \
                languages/iricGdPolydataGroup_pl_PL.ts \
                languages/iricGdPolydataGroup_pt_BR.ts \
                languages/iricGdPolydataGroup_pt_PT.ts \
                languages/iricGdPolydataGroup_ro_RO.ts \
                languages/iricGdPolydataGroup_ru_RU.ts \
                languages/iricGdPolydataGroup_sl_SI.ts \
                languages/iricGdPolydataGroup_sv_SE.ts \
                languages/iricGdPolydataGroup_th_TH.ts \
                languages/iricGdPolydataGroup_tr_TR.ts \
                languages/iricGdPolydataGroup_uk_UA.ts \
                languages/iricGdPolydataGroup_vi_VN.ts \
                languages/iricGdPolydataGroup_zh_CN.ts \
                languages/iricGdPolydataGroup_zh_TW.ts
