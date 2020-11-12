TARGET = iricGccCompoundchannel
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GCC_COMPOUNDCHANNEL_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

# iricGeoio

unix {
	LIBS += -L"../../geoio"
}
LIBS += -liricGeoio

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
HEADERS += gcc_compoundchannel_global.h \
           gridcreatingconditioncompoundchannel.h \
           gridcreatingconditioncompoundchannelabstractline.h \
           gridcreatingconditioncompoundchannelabstractpolygon.h \
           gridcreatingconditioncompoundchannelcenterline.h \
           gridcreatingconditioncompoundchannelfunctions.h \
           gridcreatingconditioncompoundchannelgridregionpolygon.h \
           gridcreatingconditioncompoundchannellowwaterchannelpolygon.h \
           gridcreatingconditioncompoundchannelsettingdialog.h \
           gridcreatingconditioncompoundchannelspline.h \
           gridcreatingconditioncreatorcompoundchannel.h \
           private/gridcreatingconditioncompoundchannel_addpolygonvertexcommand.h \
           private/gridcreatingconditioncompoundchannel_addpolylinevertexcommand.h \
           private/gridcreatingconditioncompoundchannel_definepolygonnewpointcommand.h \
           private/gridcreatingconditioncompoundchannel_definepolylinenewpointcommand.h \
           private/gridcreatingconditioncompoundchannel_editpolygoncoordinatescommand.h \
           private/gridcreatingconditioncompoundchannel_editpolylinecoordinatescommand.h \
           private/gridcreatingconditioncompoundchannel_movepolygoncommand.h \
           private/gridcreatingconditioncompoundchannel_movepolygonvertexcommand.h \
           private/gridcreatingconditioncompoundchannel_movepolylinecommand.h \
           private/gridcreatingconditioncompoundchannel_movepolylinevertexcommand.h \
           private/gridcreatingconditioncompoundchannel_polygoncoordinateseditor.h \
           private/gridcreatingconditioncompoundchannel_polylinecoordinateseditor.h \
           private/gridcreatingconditioncompoundchannel_removepolygonvertexcommand.h \
           private/gridcreatingconditioncompoundchannel_removepolylinevertexcommand.h \
           private/gridcreatingconditioncompoundchannel_switchstatuscommand.h
FORMS += gridcreatingconditioncompoundchannelsettingdialog.ui
SOURCES += gridcreatingconditioncompoundchannel.cpp \
           gridcreatingconditioncompoundchannelabstractline.cpp \
           gridcreatingconditioncompoundchannelabstractpolygon.cpp \
           gridcreatingconditioncompoundchannelcenterline.cpp \
           gridcreatingconditioncompoundchannelfunctions.cpp \
           gridcreatingconditioncompoundchannelgridregionpolygon.cpp \
           gridcreatingconditioncompoundchannellowwaterchannelpolygon.cpp \
           gridcreatingconditioncompoundchannelsettingdialog.cpp \
           gridcreatingconditioncompoundchannelspline.cpp \
           gridcreatingconditioncreatorcompoundchannel.cpp \
           private/gridcreatingconditioncompoundchannel_addpolygonvertexcommand.cpp \
           private/gridcreatingconditioncompoundchannel_addpolylinevertexcommand.cpp \
           private/gridcreatingconditioncompoundchannel_definepolygonnewpointcommand.cpp \
           private/gridcreatingconditioncompoundchannel_definepolylinenewpointcommand.cpp \
           private/gridcreatingconditioncompoundchannel_editpolygoncoordinatescommand.cpp \
           private/gridcreatingconditioncompoundchannel_editpolylinecoordinatescommand.cpp \
           private/gridcreatingconditioncompoundchannel_movepolygoncommand.cpp \
           private/gridcreatingconditioncompoundchannel_movepolygonvertexcommand.cpp \
           private/gridcreatingconditioncompoundchannel_movepolylinecommand.cpp \
           private/gridcreatingconditioncompoundchannel_movepolylinevertexcommand.cpp \
           private/gridcreatingconditioncompoundchannel_polygoncoordinateseditor.cpp \
           private/gridcreatingconditioncompoundchannel_polylinecoordinateseditor.cpp \
           private/gridcreatingconditioncompoundchannel_removepolygonvertexcommand.cpp \
           private/gridcreatingconditioncompoundchannel_removepolylinevertexcommand.cpp \
           private/gridcreatingconditioncompoundchannel_switchstatuscommand.cpp
RESOURCES += compoundchannel.qrc
TRANSLATIONS += languages/iricGccCompoundchannel_ar_EG.ts \
                languages/iricGccCompoundchannel_bg_BG.ts \
                languages/iricGccCompoundchannel_bs_BA.ts \
                languages/iricGccCompoundchannel_ca_ES.ts \
                languages/iricGccCompoundchannel_cs_CZ.ts \
                languages/iricGccCompoundchannel_da_DK.ts \
                languages/iricGccCompoundchannel_de_DE.ts \
                languages/iricGccCompoundchannel_el_GR.ts \
                languages/iricGccCompoundchannel_es_ES.ts \
                languages/iricGccCompoundchannel_et_EE.ts \
                languages/iricGccCompoundchannel_eu_ES.ts \
                languages/iricGccCompoundchannel_fi_FI.ts \
                languages/iricGccCompoundchannel_fr_FR.ts \
                languages/iricGccCompoundchannel_gl_ES.ts \
                languages/iricGccCompoundchannel_hi_IN.ts \
                languages/iricGccCompoundchannel_hu_HU.ts \
                languages/iricGccCompoundchannel_id_ID.ts \
                languages/iricGccCompoundchannel_is_IS.ts \
                languages/iricGccCompoundchannel_it_IT.ts \
                languages/iricGccCompoundchannel_ja_JP.ts \
                languages/iricGccCompoundchannel_ko_KR.ts \
                languages/iricGccCompoundchannel_ky_KG.ts \
                languages/iricGccCompoundchannel_lt_LT.ts \
                languages/iricGccCompoundchannel_lv_LV.ts \
                languages/iricGccCompoundchannel_nb_NO.ts \
                languages/iricGccCompoundchannel_nl_NL.ts \
                languages/iricGccCompoundchannel_pl_PL.ts \
                languages/iricGccCompoundchannel_pt_BR.ts \
                languages/iricGccCompoundchannel_pt_PT.ts \
                languages/iricGccCompoundchannel_ro_RO.ts \
                languages/iricGccCompoundchannel_ru_RU.ts \
                languages/iricGccCompoundchannel_sl_SI.ts \
                languages/iricGccCompoundchannel_sv_SE.ts \
                languages/iricGccCompoundchannel_th_TH.ts \
                languages/iricGccCompoundchannel_tr_TR.ts \
                languages/iricGccCompoundchannel_uk_UA.ts \
                languages/iricGccCompoundchannel_vi_VN.ts \
                languages/iricGccCompoundchannel_zh_CN.ts \
                languages/iricGccCompoundchannel_zh_TW.ts
