TARGET = iricGdPolydata
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_POLYDATA_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

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

# Post-Build Event
win32 {
	QMAKE_POST_LINK += IF NOT EXIST $(SolutionDir)\\libdlls\\$(Configuration) mkdir $(SolutionDir)\\libdlls\\$(Configuration) && copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += gd_polydata_global.h \
           geodatapolydata.h \
           geodatapolydatacolorsettingdialog.h \
           private/geodatapolydata_editnameandvaluecommand.h \
           private/geodatapolydata_impl.h \
           private/geodatapolydata_setcolorsettingcommand.h
FORMS += geodatapolydatacolorsettingdialog.ui
SOURCES += geodatapolydata.cpp \
           geodatapolydatacolorsettingdialog.cpp \
           private/geodatapolydata_editnameandvaluecommand.cpp \
           private/geodatapolydata_setcolorsettingcommand.cpp
TRANSLATIONS += languages/iricGdPolydata_ar_EG.ts \
                languages/iricGdPolydata_bg_BG.ts \
                languages/iricGdPolydata_bs_BA.ts \
                languages/iricGdPolydata_ca_ES.ts \
                languages/iricGdPolydata_cs_CZ.ts \
                languages/iricGdPolydata_da_DK.ts \
                languages/iricGdPolydata_de_DE.ts \
                languages/iricGdPolydata_el_GR.ts \
                languages/iricGdPolydata_es_ES.ts \
                languages/iricGdPolydata_et_EE.ts \
                languages/iricGdPolydata_eu_ES.ts \
                languages/iricGdPolydata_fi_FI.ts \
                languages/iricGdPolydata_fr_FR.ts \
                languages/iricGdPolydata_gl_ES.ts \
                languages/iricGdPolydata_hi_IN.ts \
                languages/iricGdPolydata_hu_HU.ts \
                languages/iricGdPolydata_id_ID.ts \
                languages/iricGdPolydata_is_IS.ts \
                languages/iricGdPolydata_it_IT.ts \
                languages/iricGdPolydata_ja_JP.ts \
                languages/iricGdPolydata_ko_KR.ts \
                languages/iricGdPolydata_ky_KG.ts \
                languages/iricGdPolydata_lt_LT.ts \
                languages/iricGdPolydata_lv_LV.ts \
                languages/iricGdPolydata_nb_NO.ts \
                languages/iricGdPolydata_nl_NL.ts \
                languages/iricGdPolydata_pl_PL.ts \
                languages/iricGdPolydata_pt_BR.ts \
                languages/iricGdPolydata_pt_PT.ts \
                languages/iricGdPolydata_ro_RO.ts \
                languages/iricGdPolydata_ru_RU.ts \
                languages/iricGdPolydata_sl_SI.ts \
                languages/iricGdPolydata_sv_SE.ts \
                languages/iricGdPolydata_th_TH.ts \
                languages/iricGdPolydata_tr_TR.ts \
                languages/iricGdPolydata_uk_UA.ts \
                languages/iricGdPolydata_vi_VN.ts \
                languages/iricGdPolydata_zh_CN.ts \
                languages/iricGdPolydata_zh_TW.ts
