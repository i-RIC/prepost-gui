TARGET = iricMeasureddata
TEMPLATE = lib
INCLUDEPATH += .

DEFINES += MEASUREDDATA_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

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
HEADERS += measureddata_api.h \
           measureddatafiledataitem.h \
           measureddatapointdataitem.h \
           measureddatapointgroupdataitem.h \
           measureddatavectordataitem.h \
           measureddatavectorgroupdataitem.h \
           measureddatavectorsetting.h \
           private/measureddatafiledataitem_impl.h \
           private/measureddatapointgroupdataitem_impl.h \
           private/measureddatapointgroupdataitem_setting.h \
           private/measureddatapointgroupdataitem_settingeditwidget.h \
           private/measureddatapointgroupdataitem_updatesettingcommand.h \
           private/measureddatavectorgroupdataitem_impl.h \
           private/measureddatavectorgroupdataitem_propertydialog.h \
           private/measureddatavectorgroupdataitem_updatesettingcommand.h
FORMS += \
         private/measureddatapointgroupdataitem_settingeditwidget.ui \
         private/measureddatavectorgroupdataitem_propertydialog.ui
SOURCES += measureddatafiledataitem.cpp \
           measureddatapointdataitem.cpp \
           measureddatapointgroupdataitem.cpp \
           measureddatavectordataitem.cpp \
           measureddatavectorgroupdataitem.cpp \
           measureddatavectorsetting.cpp \
           private/measureddatapointgroupdataitem_setting.cpp \
           private/measureddatapointgroupdataitem_settingeditwidget.cpp \
           private/measureddatapointgroupdataitem_updatesettingcommand.cpp \
           private/measureddatavectorgroupdataitem_propertydialog.cpp \
           private/measureddatavectorgroupdataitem_updatesettingcommand.cpp
TRANSLATIONS += languages/iricMeasureddata_ar_EG.ts \
                languages/iricMeasureddata_bg_BG.ts \
                languages/iricMeasureddata_bs_BA.ts \
                languages/iricMeasureddata_ca_ES.ts \
                languages/iricMeasureddata_cs_CZ.ts \
                languages/iricMeasureddata_da_DK.ts \
                languages/iricMeasureddata_de_DE.ts \
                languages/iricMeasureddata_el_GR.ts \
                languages/iricMeasureddata_es_ES.ts \
                languages/iricMeasureddata_et_EE.ts \
                languages/iricMeasureddata_eu_ES.ts \
                languages/iricMeasureddata_fi_FI.ts \
                languages/iricMeasureddata_fr_FR.ts \
                languages/iricMeasureddata_gl_ES.ts \
                languages/iricMeasureddata_hi_IN.ts \
                languages/iricMeasureddata_hu_HU.ts \
                languages/iricMeasureddata_id_ID.ts \
                languages/iricMeasureddata_is_IS.ts \
                languages/iricMeasureddata_it_IT.ts \
                languages/iricMeasureddata_ja_JP.ts \
                languages/iricMeasureddata_ko_KR.ts \
                languages/iricMeasureddata_ky_KG.ts \
                languages/iricMeasureddata_lt_LT.ts \
                languages/iricMeasureddata_lv_LV.ts \
                languages/iricMeasureddata_nb_NO.ts \
                languages/iricMeasureddata_nl_NL.ts \
                languages/iricMeasureddata_pl_PL.ts \
                languages/iricMeasureddata_pt_BR.ts \
                languages/iricMeasureddata_pt_PT.ts \
                languages/iricMeasureddata_ro_RO.ts \
                languages/iricMeasureddata_ru_RU.ts \
                languages/iricMeasureddata_sl_SI.ts \
                languages/iricMeasureddata_sv_SE.ts \
                languages/iricMeasureddata_th_TH.ts \
                languages/iricMeasureddata_tr_TR.ts \
                languages/iricMeasureddata_uk_UA.ts \
                languages/iricMeasureddata_vi_VN.ts \
                languages/iricMeasureddata_zh_CN.ts \
                languages/iricMeasureddata_zh_TW.ts
