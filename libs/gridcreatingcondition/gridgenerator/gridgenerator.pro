######################################################################
# Automatically generated by qmake (2.01a) ? 10 23 18:49:25 2014
######################################################################

TARGET = iricGccGridgenerator
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GCC_GRIDGENERATOR_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = gridgenerator_pch.h

######################
# Internal libraries #
######################

# iricMisc

unix {
	LIBS += -L"../../misc"
}
LIBS += -liricMisc

# iricGuibase

unix {
	LIBS += -L"../../guibase"
}
LIBS += -liricGuibase

# iricGuicore

unix {
	LIBS += -L"../../guicore"
}
LIBS += -liricGuicore

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonComputationalGeometry-$${VTK_MAJ_MIN} \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN} \
	-lvtkRenderingLabel-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += gcc_gridgenerator_global.h \
           gridcreatingconditioncreatorgridgenerator.h \
           gridcreatingconditiongridgenerator.h \
           gridcreatingconditiongridgeneratorsettingdialog.h
FORMS += gridcreatingconditiongridgeneratorsettingdialog.ui
SOURCES += gridcreatingconditioncreatorgridgenerator.cpp \
           gridcreatingconditiongridgenerator.cpp \
           gridcreatingconditiongridgeneratorsettingdialog.cpp
TRANSLATIONS += languages/iricGccGridgenerator_ar_EG.ts \
                languages/iricGccGridgenerator_bg_BG.ts \
                languages/iricGccGridgenerator_bs_BA.ts \
                languages/iricGccGridgenerator_ca_ES.ts \
                languages/iricGccGridgenerator_cs_CZ.ts \
                languages/iricGccGridgenerator_da_DK.ts \
                languages/iricGccGridgenerator_de_DE.ts \
                languages/iricGccGridgenerator_el_GR.ts \
                languages/iricGccGridgenerator_es_ES.ts \
                languages/iricGccGridgenerator_et_EE.ts \
                languages/iricGccGridgenerator_eu_ES.ts \
                languages/iricGccGridgenerator_fi_FI.ts \
                languages/iricGccGridgenerator_fr_FR.ts \
                languages/iricGccGridgenerator_gl_ES.ts \
                languages/iricGccGridgenerator_hi_IN.ts \
                languages/iricGccGridgenerator_hu_HU.ts \
                languages/iricGccGridgenerator_id_ID.ts \
                languages/iricGccGridgenerator_is_IS.ts \
                languages/iricGccGridgenerator_it_IT.ts \
                languages/iricGccGridgenerator_ja_JP.ts \
                languages/iricGccGridgenerator_ko_KR.ts \
                languages/iricGccGridgenerator_ky_KG.ts \
                languages/iricGccGridgenerator_lt_LT.ts \
                languages/iricGccGridgenerator_lv_LV.ts \
                languages/iricGccGridgenerator_nb_NO.ts \
                languages/iricGccGridgenerator_nl_NL.ts \
                languages/iricGccGridgenerator_pl_PL.ts \
                languages/iricGccGridgenerator_pt_BR.ts \
                languages/iricGccGridgenerator_pt_PT.ts \
                languages/iricGccGridgenerator_ro_RO.ts \
                languages/iricGccGridgenerator_ru_RU.ts \
                languages/iricGccGridgenerator_sl_SI.ts \
                languages/iricGccGridgenerator_sv_SE.ts \
                languages/iricGccGridgenerator_th_TH.ts \
                languages/iricGccGridgenerator_tr_TR.ts \
                languages/iricGccGridgenerator_uk_UA.ts \
                languages/iricGccGridgenerator_vi_VN.ts \
                languages/iricGccGridgenerator_zh_CN.ts \
                languages/iricGccGridgenerator_zh_TW.ts
