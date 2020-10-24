TARGET = iricAxis2d
TEMPLATE = lib
INCLUDEPATH += .

DEFINES += AXIS2D_LIBRARY

include( ../../../paths.pri )

QT += widgets

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

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkInteractionWidgets-6.1 \
	-lvtkRenderingAnnotation-6.1 \
	-lvtkRenderingCore-6.1

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += axis2d_api.h axis2ddataitem.h private/axis2ddataitem_impl.h
SOURCES += axis2ddataitem.cpp
TRANSLATIONS += languages/iricAxis2d_ar_EG.ts \
                languages/iricAxis2d_bg_BG.ts \
                languages/iricAxis2d_bs_BA.ts \
                languages/iricAxis2d_ca_ES.ts \
                languages/iricAxis2d_cs_CZ.ts \
                languages/iricAxis2d_da_DK.ts \
                languages/iricAxis2d_de_DE.ts \
                languages/iricAxis2d_el_GR.ts \
                languages/iricAxis2d_es_ES.ts \
                languages/iricAxis2d_et_EE.ts \
                languages/iricAxis2d_eu_ES.ts \
                languages/iricAxis2d_fi_FI.ts \
                languages/iricAxis2d_fr_FR.ts \
                languages/iricAxis2d_gl_ES.ts \
                languages/iricAxis2d_hi_IN.ts \
                languages/iricAxis2d_hu_HU.ts \
                languages/iricAxis2d_id_ID.ts \
                languages/iricAxis2d_is_IS.ts \
                languages/iricAxis2d_it_IT.ts \
                languages/iricAxis2d_ja_JP.ts \
                languages/iricAxis2d_ko_KR.ts \
                languages/iricAxis2d_ky_KG.ts \
                languages/iricAxis2d_lt_LT.ts \
                languages/iricAxis2d_lv_LV.ts \
                languages/iricAxis2d_nb_NO.ts \
                languages/iricAxis2d_nl_NL.ts \
                languages/iricAxis2d_pl_PL.ts \
                languages/iricAxis2d_pt_BR.ts \
                languages/iricAxis2d_pt_PT.ts \
                languages/iricAxis2d_ro_RO.ts \
                languages/iricAxis2d_ru_RU.ts \
                languages/iricAxis2d_sl_SI.ts \
                languages/iricAxis2d_sv_SE.ts \
                languages/iricAxis2d_th_TH.ts \
                languages/iricAxis2d_tr_TR.ts \
                languages/iricAxis2d_uk_UA.ts \
                languages/iricAxis2d_vi_VN.ts \
                languages/iricAxis2d_zh_CN.ts \
                languages/iricAxis2d_zh_TW.ts
