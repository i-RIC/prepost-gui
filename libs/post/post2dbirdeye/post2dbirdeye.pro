TARGET = iricPost2dbirdeye
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += POST2DBIRDEYE_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = post2dbirdeye_pch.h

######################
# Internal libraries #
######################

# iricAxis3d

unix {
	LIBS += -L"../../dataitem/axis3d"
}
LIBS += -liricAxis3d

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

# iricPostbase

unix {
	LIBS += -L"../../postbase"
}
LIBS += -liricPostbase


# iricPost2d

unix {
	LIBS += -L"../post2d"
}
LIBS += -liricPost2d

# iricLogo

unix {
	LIBS += -L"../../dataitem/logo"
}
LIBS += -liricLogo

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkCommonExecutionModel-$${VTK_MAJ_MIN} \
	-lvtkCommonMath-$${VTK_MAJ_MIN} \
	-lvtkCommonMisc-$${VTK_MAJ_MIN} \
	-lvtkCommonTransforms-$${VTK_MAJ_MIN} \
	-lvtkFiltersCore-$${VTK_MAJ_MIN} \
	-lvtkFiltersGeometry-$${VTK_MAJ_MIN} \
	-lvtkFiltersGeneral-$${VTK_MAJ_MIN} \
	-lvtkFiltersExtraction-$${VTK_MAJ_MIN} \
	-lvtkFiltersFlowPaths-$${VTK_MAJ_MIN} \
	-lvtkFiltersSources-$${VTK_MAJ_MIN} \
	-lvtkFiltersTexture-$${VTK_MAJ_MIN} \
	-lvtkGUISupportQt-$${VTK_MAJ_MIN} \
	-lvtkInteractionWidgets-$${VTK_MAJ_MIN} \
	-lvtkIOImage-$${VTK_MAJ_MIN} \
	-lvtkIOCore-$${VTK_MAJ_MIN} \
	-lvtkIOLegacy-$${VTK_MAJ_MIN} \
	-lvtkRenderingAnnotation-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN} \
	-lvtkRenderingFreeType-$${VTK_MAJ_MIN} \
	-lvtkRenderingLabel-$${VTK_MAJ_MIN} \
	-lvtkRenderingLOD-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += post2dbirdeye_global.h \
           post2dbirdeyeobjectbrowser.h \
           post2dbirdeyeobjectbrowserview.h \
           post2dbirdeyewindow.h \
           post2dbirdeyewindowactionmanager.h \
           post2dbirdeyewindowdataitem.h \
           post2dbirdeyewindowdatamodel.h \
           post2dbirdeyewindowgraphicsview.h \
           post2dbirdeyewindowprojectdataitem.h \
           datamodel/post2dbirdeyewindowcellscalargroupdataitem.h \
           datamodel/post2dbirdeyewindowcellscalargrouptopdataitem.h \
           datamodel/post2dbirdeyewindowgridshapedataitem.h \
           datamodel/post2dbirdeyewindowgridtypedataitem.h \
           datamodel/post2dbirdeyewindownodescalargroupdataitem.h \
           datamodel/post2dbirdeyewindownodescalargrouptopdataitem.h \
           datamodel/post2dbirdeyewindowrootdataitem.h \
           datamodel/post2dbirdeyewindowzonedataitem.h \
           datamodel/private/post2dbirdeyewindowcellscalargroupdataitem_impl.h \
           datamodel/private/post2dbirdeyewindowcellscalargroupdataitem_setting.h \
           datamodel/private/post2dbirdeyewindowcellscalargroupdataitem_settingeditwidget.h \
           datamodel/private/post2dbirdeyewindowcellscalargroupdataitem_updatesettingcommand.h \
           datamodel/private/post2dbirdeyewindowgridshapedataitem_impl.h \
           datamodel/private/post2dbirdeyewindowgridshapedataitem_setting.h \
           datamodel/private/post2dbirdeyewindowgridshapedataitem_settingeditwidget.h \
           datamodel/private/post2dbirdeyewindownodescalargroupdataitem_impl.h \
           datamodel/private/post2dbirdeyewindownodescalargroupdataitem_setting.h \
           datamodel/private/post2dbirdeyewindownodescalargroupdataitem_settingeditwidget.h \
           datamodel/private/post2dbirdeyewindownodescalargroupdataitem_updatesettingcommand.h
FORMS += datamodel/private/post2dbirdeyewindowcellscalargroupdataitem_settingeditwidget.ui \
         datamodel/private/post2dbirdeyewindowgridshapedataitem_settingeditwidget.ui \
         datamodel/private/post2dbirdeyewindownodescalargroupdataitem_settingeditwidget.ui
SOURCES += post2dbirdeyeobjectbrowser.cpp \
           post2dbirdeyeobjectbrowserview.cpp \
           post2dbirdeyewindow.cpp \
           post2dbirdeyewindowactionmanager.cpp \
           post2dbirdeyewindowdataitem.cpp \
           post2dbirdeyewindowdatamodel.cpp \
           post2dbirdeyewindowgraphicsview.cpp \
           post2dbirdeyewindowprojectdataitem.cpp \
           datamodel/post2dbirdeyewindowcellscalargroupdataitem.cpp \
           datamodel/post2dbirdeyewindowcellscalargrouptopdataitem.cpp \
           datamodel/post2dbirdeyewindowgridshapedataitem.cpp \
           datamodel/post2dbirdeyewindowgridtypedataitem.cpp \
           datamodel/post2dbirdeyewindownodescalargroupdataitem.cpp \
           datamodel/post2dbirdeyewindownodescalargrouptopdataitem.cpp \
           datamodel/post2dbirdeyewindowrootdataitem.cpp \
           datamodel/post2dbirdeyewindowzonedataitem.cpp \
           datamodel/private/post2dbirdeyewindowcellscalargroupdataitem_impl.cpp \
           datamodel/private/post2dbirdeyewindowcellscalargroupdataitem_setting.cpp \
           datamodel/private/post2dbirdeyewindowcellscalargroupdataitem_settingeditwidget.cpp \
           datamodel/private/post2dbirdeyewindowcellscalargroupdataitem_updatesettingcommand.cpp \
           datamodel/private/post2dbirdeyewindowgridshapedataitem_impl.cpp \
           datamodel/private/post2dbirdeyewindowgridshapedataitem_setting.cpp \
           datamodel/private/post2dbirdeyewindowgridshapedataitem_settingeditwidget.cpp \
           datamodel/private/post2dbirdeyewindownodescalargroupdataitem_impl.cpp \
           datamodel/private/post2dbirdeyewindownodescalargroupdataitem_setting.cpp \
           datamodel/private/post2dbirdeyewindownodescalargroupdataitem_settingeditwidget.cpp \
           datamodel/private/post2dbirdeyewindownodescalargroupdataitem_updatesettingcommand.cpp
RESOURCES += post2dbirdeye.qrc
TRANSLATIONS += languages/iricPost2dbirdeye_ar_EG.ts \
                languages/iricPost2dbirdeye_bg_BG.ts \
                languages/iricPost2dbirdeye_bs_BA.ts \
                languages/iricPost2dbirdeye_ca_ES.ts \
                languages/iricPost2dbirdeye_cs_CZ.ts \
                languages/iricPost2dbirdeye_da_DK.ts \
                languages/iricPost2dbirdeye_de_DE.ts \
                languages/iricPost2dbirdeye_el_GR.ts \
                languages/iricPost2dbirdeye_es_ES.ts \
                languages/iricPost2dbirdeye_et_EE.ts \
                languages/iricPost2dbirdeye_eu_ES.ts \
                languages/iricPost2dbirdeye_fi_FI.ts \
                languages/iricPost2dbirdeye_fr_FR.ts \
                languages/iricPost2dbirdeye_gl_ES.ts \
                languages/iricPost2dbirdeye_hi_IN.ts \
                languages/iricPost2dbirdeye_hu_HU.ts \
                languages/iricPost2dbirdeye_id_ID.ts \
                languages/iricPost2dbirdeye_is_IS.ts \
                languages/iricPost2dbirdeye_it_IT.ts \
                languages/iricPost2dbirdeye_ja_JP.ts \
                languages/iricPost2dbirdeye_ko_KR.ts \
                languages/iricPost2dbirdeye_ky_KG.ts \
                languages/iricPost2dbirdeye_lt_LT.ts \
                languages/iricPost2dbirdeye_lv_LV.ts \
                languages/iricPost2dbirdeye_nb_NO.ts \
                languages/iricPost2dbirdeye_nl_NL.ts \
                languages/iricPost2dbirdeye_pl_PL.ts \
                languages/iricPost2dbirdeye_pt_BR.ts \
                languages/iricPost2dbirdeye_pt_PT.ts \
                languages/iricPost2dbirdeye_ro_RO.ts \
                languages/iricPost2dbirdeye_ru_RU.ts \
                languages/iricPost2dbirdeye_sl_SI.ts \
                languages/iricPost2dbirdeye_sv_SE.ts \
                languages/iricPost2dbirdeye_th_TH.ts \
                languages/iricPost2dbirdeye_tr_TR.ts \
                languages/iricPost2dbirdeye_uk_UA.ts \
                languages/iricPost2dbirdeye_vi_VN.ts \
                languages/iricPost2dbirdeye_zh_CN.ts \
                languages/iricPost2dbirdeye_zh_TW.ts
