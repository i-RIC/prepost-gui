TARGET = iricAxis3d
TEMPLATE = lib
INCLUDEPATH += .

DEFINES += AXIS3D_LIBRARY

include( ../../../paths.pri )

QT += widgets

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

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkInteractionWidgets-6.1 \
	-lvtkRenderingAnnotation-6.1 \
	-lvtkRenderingCore-6.1

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += axis3d_api.h axis3ddataitem.h private/axis3ddataitem_impl.h
SOURCES += axis3ddataitem.cpp
TRANSLATIONS += languages/iricAxis3d_ar_EG.ts \
                languages/iricAxis3d_bg_BG.ts \
                languages/iricAxis3d_bs_BA.ts \
                languages/iricAxis3d_ca_ES.ts \
                languages/iricAxis3d_cs_CZ.ts \
                languages/iricAxis3d_da_DK.ts \
                languages/iricAxis3d_de_DE.ts \
                languages/iricAxis3d_el_GR.ts \
                languages/iricAxis3d_es_ES.ts \
                languages/iricAxis3d_et_EE.ts \
                languages/iricAxis3d_eu_ES.ts \
                languages/iricAxis3d_fi_FI.ts \
                languages/iricAxis3d_fr_FR.ts \
                languages/iricAxis3d_gl_ES.ts \
                languages/iricAxis3d_hi_IN.ts \
                languages/iricAxis3d_hu_HU.ts \
                languages/iricAxis3d_id_ID.ts \
                languages/iricAxis3d_is_IS.ts \
                languages/iricAxis3d_it_IT.ts \
                languages/iricAxis3d_ja_JP.ts \
                languages/iricAxis3d_ko_KR.ts \
                languages/iricAxis3d_ky_KG.ts \
                languages/iricAxis3d_lt_LT.ts \
                languages/iricAxis3d_lv_LV.ts \
                languages/iricAxis3d_nb_NO.ts \
                languages/iricAxis3d_nl_NL.ts \
                languages/iricAxis3d_pl_PL.ts \
                languages/iricAxis3d_pt_BR.ts \
                languages/iricAxis3d_pt_PT.ts \
                languages/iricAxis3d_ro_RO.ts \
                languages/iricAxis3d_ru_RU.ts \
                languages/iricAxis3d_sl_SI.ts \
                languages/iricAxis3d_sv_SE.ts \
                languages/iricAxis3d_th_TH.ts \
                languages/iricAxis3d_tr_TR.ts \
                languages/iricAxis3d_uk_UA.ts \
                languages/iricAxis3d_vi_VN.ts \
                languages/iricAxis3d_zh_CN.ts \
                languages/iricAxis3d_zh_TW.ts
