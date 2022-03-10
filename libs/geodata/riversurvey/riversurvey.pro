TARGET = iricGdRiversurvey
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_RIVERSURVEY_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = riversurvey_pch.h

######################
# Internal libraries #
######################

# iricGdPointmap

unix {
        LIBS += -L"../pointmap"
}
LIBS += -liricGdPointmap

# iricGdPolydata

unix {
        LIBS += -L"../polydata"
}
LIBS += -liricGdPolydata

# iricGdPolydataGroup

unix {
        LIBS += -L"../polydatagroup"
}
LIBS += -liricGdPolydataGroup

# iricMisc

unix {
	LIBS += -L"../../misc"
}
LIBS += -liricMisc

# iricGeoio

unix {
        LIBS += -L"../../geoio"
}
LIBS += -liricGeoio

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

# iricGdPolyLine

unix {
	LIBS += -L"../../geodata/polyline"
}
LIBS += -liricGdPolyLine

# iricGdPolyLineGroup

unix {
	LIBS += -L"../../geodata/polylinegroup"
}
LIBS += -liricGdPolyLineGroup

# iricHdRiversurveywaterelevation

unix {
	LIBS += -L"../../hydraulicdata/riversurveywaterelevation"
}
LIBS += -liricHdRiversurveywaterelevation

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkCommonExecutionModel-$${VTK_MAJ_MIN} \
	-lvtkFiltersExtraction-$${VTK_MAJ_MIN} \
	-lvtkFiltersGeometry-$${VTK_MAJ_MIN} \
	-lvtkIOCore-$${VTK_MAJ_MIN} \
	-lvtkIOLegacy-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN} \
	-lvtkRenderingLabel-$${VTK_MAJ_MIN} \
	-lvtkRenderingLOD-$${VTK_MAJ_MIN}

# geos

LIBS += -lgeos

# iriclib

win32 {
    CONFIG(debug, debug|release) {
        LIBS += -liriclibd
    } else {
        LIBS += -liriclib
    }
}

unix {
    LIBS += -liriclib
}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += gd_riversurvey_global.h \
           geodatarivercrosssection.h \
           geodatarivercrosssectionaltitudemovedialog.h \
           geodatariverpathpoint.h \
           geodatariverpathpointexpanddialog.h \
           geodatariverpathpointextensionadddialog.h \
           geodatariverpathpointinsertdialog.h \
           geodatariverpathpointmovedialog.h \
           geodatariverpathpointrenamedialog.h \
           geodatariverpathpointrotatedialog.h \
           geodatariverpathpointshiftdialog.h \
           geodatarivershapeinterpolator.h \
           geodatariversurvey.h \
           geodatariversurveybackgroundgridcreatethread.h \
           geodatariversurveybuildbanklinesdialog.h \
           geodatariversurveycreator.h \
           geodatariversurveycrosssectiondisplaysetting.h \
           geodatariversurveycrosssectiondisplaysettingdialog.h \
           geodatariversurveycrosssectioneditfrompointdialog.h \
           geodatariversurveycrosssectionwindow.h \
           geodatariversurveycrosssectionwindowgraphicsview.h \
           geodatariversurveycrosssectionwindowprojectdataitem.h \
           geodatariversurveycrosssectionwindowtableview.h \
           geodatariversurveycrosssectionwindowwsetablewidget.h \
           geodatariversurveyctrlpointbackup.h \
           geodatariversurveydisplaysetting.h \
           geodatariversurveydisplaysettingdialog.h \
           geodatariversurveyexporter.h \
           geodatariversurveygeneratedialog.h \
           geodatariversurveyhonryuexporter.h \
           geodatariversurveyhonryuexportersettingdialog.h \
           geodatariversurveyimporter.h \
           geodatariversurveyimportersettingdialog.h \
           geodatariversurveylandxmlexporter.h \
           geodatariversurveymappointsdialog.h \
           geodatariversurveymlitimporter.h \
           geodatariversurveynodemapper.h \
           geodatariversurveyproxy.h \
           geodatariversurveyvtkexporter.h \
           gridcreatingconditionriversurveyinterface.h \
           private/geodatariversurvey_addextensioncommand.h \
           private/geodatariversurvey_changeselectioncommand.h \
           private/geodatariversurvey_deleteriverpathpointcommand.h \
           private/geodatariversurvey_editcrosssectioncommand.h \
           private/geodatariversurvey_expandcrosssectioncommand.h \
           private/geodatariversurvey_impl.h \
           private/geodatariversurvey_insertriverpathpointcommand.h \
           private/geodatariversurvey_mouseeditcrosssectioncommand.h \
           private/geodatariversurvey_mousemoveextensioncommand.h \
           private/geodatariversurvey_mouserotaterivercrosssectioncommand.h \
           private/geodatariversurvey_mouseshiftriverpathcentercommand.h \
           private/geodatariversurvey_moveriverpathpointcommand.h \
           private/geodatariversurvey_polylinecoordinateseditor.h \
           private/geodatariversurvey_polylinefinishdefiningcommand.h \
           private/geodatariversurvey_polylineupdatelabelscommand.h \
           private/geodatariversurvey_removeextensioncommand.h \
           private/geodatariversurvey_renameriverpathpointcommand.h \
           private/geodatariversurvey_rotaterivercrosssectioncommand.h \
           private/geodatariversurvey_setdisplaysettingcommand.h \
           private/geodatariversurvey_shiftriverpathcentercommand.h \
           private/geodatariversurvey_translateriverpathpointcommand.h \
           private/geodatariversurveycrosssectionwindow_datatabledelegate.h \
           private/geodatariversurveycrosssectionwindow_impl.h \
           private/geodatariversurveycrosssectionwindow_riversurveytabledelegate.h \
           private/geodatariversurveycrosssectionwindow_wsetabledelegate.h \
           private/geodatariversurveycrosssectionwindowgraphicsview_setdisplaysettingcommand.h \
           private/geodatariversurveyimporter_problemsdialog.h \
           private/geodatariversurveymlitimporter_problemsdialog.h \
           private/geodatariversurveyproxy_setsettingcommand.h
FORMS += geodatarivercrosssectionaltitudemovedialog.ui \
         geodatariverpathpointexpanddialog.ui \
         geodatariverpathpointextensionadddialog.ui \
         geodatariverpathpointinsertdialog.ui \
         geodatariverpathpointmovedialog.ui \
         geodatariverpathpointrenamedialog.ui \
         geodatariverpathpointrotatedialog.ui \
         geodatariverpathpointshiftdialog.ui \
         geodatariversurveybuildbanklinesdialog.ui \
         geodatariversurveycrosssectiondisplaysettingdialog.ui \
         geodatariversurveycrosssectioneditfrompointdialog.ui \
         geodatariversurveycrosssectionwindow.ui \
         geodatariversurveydisplaysettingdialog.ui \
         geodatariversurveygeneratedialog.ui \
         geodatariversurveyhonryuexportersettingdialog.ui \
         geodatariversurveyimportersettingdialog.ui \
         geodatariversurveymappointsdialog.ui \
         private/geodatariversurveyimporter_problemsdialog.ui \
         private/geodatariversurveymlitimporter_problemsdialog.ui
SOURCES += geodatarivercrosssection.cpp \
           geodatarivercrosssectionaltitudemovedialog.cpp \
           geodatariverpathpoint.cpp \
           geodatariverpathpointexpanddialog.cpp \
           geodatariverpathpointextensionadddialog.cpp \
           geodatariverpathpointinsertdialog.cpp \
           geodatariverpathpointmovedialog.cpp \
           geodatariverpathpointrenamedialog.cpp \
           geodatariverpathpointrotatedialog.cpp \
           geodatariverpathpointshiftdialog.cpp \
           geodatarivershapeinterpolator.cpp \
           geodatariversurvey.cpp \
           geodatariversurveybackgroundgridcreatethread.cpp \
           geodatariversurveybuildbanklinesdialog.cpp \
           geodatariversurveycreator.cpp \
           geodatariversurveycrosssectiondisplaysetting.cpp \
           geodatariversurveycrosssectiondisplaysettingdialog.cpp \
           geodatariversurveycrosssectioneditfrompointdialog.cpp \
           geodatariversurveycrosssectionwindow.cpp \
           geodatariversurveycrosssectionwindowgraphicsview.cpp \
           geodatariversurveycrosssectionwindowprojectdataitem.cpp \
           geodatariversurveycrosssectionwindowtableview.cpp \
           geodatariversurveycrosssectionwindowwsetablewidget.cpp \
           geodatariversurveyctrlpointbackup.cpp \
           geodatariversurveydisplaysetting.cpp \
           geodatariversurveydisplaysettingdialog.cpp \
           geodatariversurveyexporter.cpp \
           geodatariversurveygeneratedialog.cpp \
           geodatariversurveyhonryuexporter.cpp \
           geodatariversurveyhonryuexportersettingdialog.cpp \
           geodatariversurveyimporter.cpp \
           geodatariversurveyimportersettingdialog.cpp \
           geodatariversurveylandxmlexporter.cpp \
           geodatariversurveymappointsdialog.cpp \
           geodatariversurveymlitimporter.cpp \
           geodatariversurveynodemapper.cpp \
           geodatariversurveyproxy.cpp \
           geodatariversurveyvtkexporter.cpp \
           private/geodatariversurvey_addextensioncommand.cpp \
           private/geodatariversurvey_changeselectioncommand.cpp \
           private/geodatariversurvey_deleteriverpathpointcommand.cpp \
           private/geodatariversurvey_editcrosssectioncommand.cpp \
           private/geodatariversurvey_expandcrosssectioncommand.cpp \
           private/geodatariversurvey_impl.cpp \
           private/geodatariversurvey_insertriverpathpointcommand.cpp \
           private/geodatariversurvey_mouseeditcrosssectioncommand.cpp \
           private/geodatariversurvey_mousemoveextensioncommand.cpp \
           private/geodatariversurvey_mouserotaterivercrosssectioncommand.cpp \
           private/geodatariversurvey_mouseshiftriverpathcentercommand.cpp \
           private/geodatariversurvey_moveriverpathpointcommand.cpp \
           private/geodatariversurvey_polylinecoordinateseditor.cpp \
           private/geodatariversurvey_polylinefinishdefiningcommand.cpp \
           private/geodatariversurvey_polylineupdatelabelscommand.cpp \
           private/geodatariversurvey_removeextensioncommand.cpp \
           private/geodatariversurvey_renameriverpathpointcommand.cpp \
           private/geodatariversurvey_rotaterivercrosssectioncommand.cpp \
           private/geodatariversurvey_setdisplaysettingcommand.cpp \
           private/geodatariversurvey_shiftriverpathcentercommand.cpp \
           private/geodatariversurvey_translateriverpathpointcommand.cpp \
           private/geodatariversurveycrosssectionwindow_datatabledelegate.cpp \
           private/geodatariversurveycrosssectionwindow_riversurveytabledelegate.cpp \
           private/geodatariversurveycrosssectionwindow_wsetabledelegate.cpp \
           private/geodatariversurveycrosssectionwindowgraphicsview_setdisplaysettingcommand.cpp \
           private/geodatariversurveyimporter_problemsdialog.cpp \
           private/geodatariversurveymlitimporter_problemsdialog.cpp \
           private/geodatariversurveyproxy_setsettingcommand.cpp
RESOURCES += riversurvey.qrc
TRANSLATIONS += languages/iricGdRiversurvey_ar_EG.ts \
                languages/iricGdRiversurvey_bg_BG.ts \
                languages/iricGdRiversurvey_bs_BA.ts \
                languages/iricGdRiversurvey_ca_ES.ts \
                languages/iricGdRiversurvey_cs_CZ.ts \
                languages/iricGdRiversurvey_da_DK.ts \
                languages/iricGdRiversurvey_de_DE.ts \
                languages/iricGdRiversurvey_el_GR.ts \
                languages/iricGdRiversurvey_es_ES.ts \
                languages/iricGdRiversurvey_et_EE.ts \
                languages/iricGdRiversurvey_eu_ES.ts \
                languages/iricGdRiversurvey_fi_FI.ts \
                languages/iricGdRiversurvey_fr_FR.ts \
                languages/iricGdRiversurvey_gl_ES.ts \
                languages/iricGdRiversurvey_hi_IN.ts \
                languages/iricGdRiversurvey_hu_HU.ts \
                languages/iricGdRiversurvey_id_ID.ts \
                languages/iricGdRiversurvey_is_IS.ts \
                languages/iricGdRiversurvey_it_IT.ts \
                languages/iricGdRiversurvey_ja_JP.ts \
                languages/iricGdRiversurvey_ko_KR.ts \
                languages/iricGdRiversurvey_ky_KG.ts \
                languages/iricGdRiversurvey_lt_LT.ts \
                languages/iricGdRiversurvey_lv_LV.ts \
                languages/iricGdRiversurvey_nb_NO.ts \
                languages/iricGdRiversurvey_nl_NL.ts \
                languages/iricGdRiversurvey_pl_PL.ts \
                languages/iricGdRiversurvey_pt_BR.ts \
                languages/iricGdRiversurvey_pt_PT.ts \
                languages/iricGdRiversurvey_ro_RO.ts \
                languages/iricGdRiversurvey_ru_RU.ts \
                languages/iricGdRiversurvey_sl_SI.ts \
                languages/iricGdRiversurvey_sv_SE.ts \
                languages/iricGdRiversurvey_th_TH.ts \
                languages/iricGdRiversurvey_tr_TR.ts \
                languages/iricGdRiversurvey_uk_UA.ts \
                languages/iricGdRiversurvey_vi_VN.ts \
                languages/iricGdRiversurvey_zh_CN.ts \
                languages/iricGdRiversurvey_zh_TW.ts
