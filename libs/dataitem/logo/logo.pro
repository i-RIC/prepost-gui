TARGET = iricLogo
TEMPLATE = lib
INCLUDEPATH += .

DEFINES += LOGO_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = logo_pch.h

######################
# Internal libraries #
######################

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

# iricMisc

unix {
	LIBS += -L"../../misc"
}
LIBS += -liricMisc

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkCommonExecutionModel-$${VTK_MAJ_MIN} \
	-lvtkCommonMisc-$${VTK_MAJ_MIN} \
	-lvtkFiltersCore-$${VTK_MAJ_MIN} \
	-lvtkFiltersGeneral-$${VTK_MAJ_MIN} \
	-lvtkFiltersGeometry-$${VTK_MAJ_MIN} \
	-lvtkFiltersSources-$${VTK_MAJ_MIN} \
	-lvtkInteractionWidgets-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN} \
	-lvtkRenderingFreeType-$${VTK_MAJ_MIN} \
	-lvtkRenderingLOD-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += logo_api.h \
           logo_pch.h \
           logodataitem.h \
           logosetting.h \
           logosettingeditwidget.h \
           private/logodataitem_impl.h \
           private/logosetting_imagebuilder.h
FORMS += logosettingeditwidget.ui
SOURCES += logodataitem.cpp \
           logosetting.cpp \
           logosettingeditwidget.cpp \
           private/logodataitem_impl.cpp \
           private/logosetting_imagebuilder.cpp
RESOURCES += logo.qrc
TRANSLATIONS += languages/iricLogo_ar_EG.ts \
                languages/iricLogo_bg_BG.ts \
                languages/iricLogo_bs_BA.ts \
                languages/iricLogo_ca_ES.ts \
                languages/iricLogo_cs_CZ.ts \
                languages/iricLogo_da_DK.ts \
                languages/iricLogo_de_DE.ts \
                languages/iricLogo_el_GR.ts \
                languages/iricLogo_es_ES.ts \
                languages/iricLogo_et_EE.ts \
                languages/iricLogo_eu_ES.ts \
                languages/iricLogo_fi_FI.ts \
                languages/iricLogo_fr_FR.ts \
                languages/iricLogo_gl_ES.ts \
                languages/iricLogo_hi_IN.ts \
                languages/iricLogo_hu_HU.ts \
                languages/iricLogo_id_ID.ts \
                languages/iricLogo_is_IS.ts \
                languages/iricLogo_it_IT.ts \
                languages/iricLogo_ja_JP.ts \
                languages/iricLogo_ko_KR.ts \
                languages/iricLogo_ky_KG.ts \
                languages/iricLogo_lt_LT.ts \
                languages/iricLogo_lv_LV.ts \
                languages/iricLogo_nb_NO.ts \
                languages/iricLogo_nl_NL.ts \
                languages/iricLogo_pl_PL.ts \
                languages/iricLogo_pt_BR.ts \
                languages/iricLogo_pt_PT.ts \
                languages/iricLogo_ro_RO.ts \
                languages/iricLogo_ru_RU.ts \
                languages/iricLogo_sl_SI.ts \
                languages/iricLogo_sv_SE.ts \
                languages/iricLogo_th_TH.ts \
                languages/iricLogo_tr_TR.ts \
                languages/iricLogo_uk_UA.ts \
                languages/iricLogo_vi_VN.ts \
                languages/iricLogo_zh_CN.ts \
                languages/iricLogo_zh_TW.ts

FORMS += \
    logosettingeditwidget.ui
