TARGET = iricMisc
TEMPLATE = lib

DEFINES += MISC_LIBRARY

QT += network widgets xml

include( ../../paths.pri )

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = misc_pch.h

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += anms.h \
           boolcontainer.h \
           colorcontainer.h \
           compositecontainer.h \
           csviohelper.h \
           doublecontainer.h \
           doublemappingsetting.h \
           dvector.h \
           edge.h \
           enumcontainert.h \
           errormessage.h \
           fileremover.h \
           filesystemfunction.h \
           folderremover.h \
           informationdialog.h \
           intarraycontainer.h \
           intcontainer.h \
           integerrangecontainer.h \
           interpolator.h \
           intvectorcontainer.h \
           iricrootpath.h \
           iricundostack.h \
           keyboardsupport.h \
           lastiodirectory.h \
           latitudelongitude.h \
           latitudelongitudeangle.h \
           linearinterpolator.h \
           mathsupport.h \
           mergesupportedlistcommand.h \
           misc_global.h \
           modifycommanddialog.h \
           modifycommandwidget.h \
           mouseeventcontroller.h \
           multicelloperabletableview.h \
           networksetting.h \
           numericvaluecontainert.h \
           opacitycontainer.h \
           orderedvalueselectdialog.h \
           primitiveview.h \
           pythonutil.h \
           qfontcontainer.h \
           qfontdisplaylabel.h \
           qfonteditwidget.h \
           qimagedisplaywidget.h \
           qlabelwithclickedsignal.h \
           qmdiareawithscrollbehaviormodification.h \
           qpointfcompare.h \
           qpointfcontainer.h \
           qspinboxwithfocussignals.h \
           qtimezoneselectcombobox.h \
           qttool.h \
           qundocommandhelper.h \
           qwidgetcontainer.h \
           rangecontainert.h \
           rectregion.h \
           simplevaluecontainert.h \
           slowmotionspeededitdialog.h \
           splineinterpolator.h \
           standarderrormessages.h \
           stringcontainer.h \
           stringtool.h \
           threadwithprogressinfo.h \
           tpoexporter.h \
           valuearraycontainert.h \
           valuechangert.h \
           valuemodifycommandt.h \
           valuerangecontainer.h \
           valuerestorert.h \
           valueselectdialog.h \
           versionnumber.h \
           windowgeometrycontainer.h \
           windowsizeeditwidget.h \
           windowsizestandardsetting.h \
           windowsizestandardsettingeditdialog.h \
           xmlattributecontainer.h \
           xmlsupport.h \
           zdepthrange.h \
           ziparchive.h \
           private/colorcontainer_impl.h \
           private/compositecontainer_impl.h \
           private/edge_detail.h \
           private/enumcontainert_detail.h \
           private/latitudelongitude_impl.h \
           private/latitudelongitudeangle_impl.h \
           private/networksetting_impl.h \
           private/numericvaluecontainert_detail.h \
           private/qpointfcontainer_impl.h \
           private/rangecontainert_detail.h \
           private/simplevaluecontainert_detail.h \
           private/valuearraycontainert_detail.h \
           private/valuechangert_detail.h \
           private/valuemodifycommandt_detail.h \
           private/valuerestorert_detail.h \
           private/versionnumber_impl.h \
           private/xmlattributecontainer_impl.h \
           private/zdepthrange_impl.h
FORMS += informationdialog.ui \
         modifycommanddialog.ui \
         orderedvalueselectdialog.ui \
         primitiveview.ui \
         qfonteditwidget.ui \
         slowmotionspeededitdialog.ui \
         valueselectdialog.ui \
         windowsizeeditwidget.ui \
         windowsizestandardsettingeditdialog.ui
SOURCES += anms.cpp \
           boolcontainer.cpp \
           colorcontainer.cpp \
           compositecontainer.cpp \
           csviohelper.cpp \
           doublecontainer.cpp \
           errormessage.cpp \
           fileremover.cpp \
           filesystemfunction.cpp \
           folderremover.cpp \
           informationdialog.cpp \
           intarraycontainer.cpp \
           intcontainer.cpp \
           integerrangecontainer.cpp \
           interpolator.cpp \
           intvectorcontainer.cpp \
           iricrootpath.cpp \
           iricundostack.cpp \
           keyboardsupport.cpp \
           lastiodirectory.cpp \
           latitudelongitude.cpp \
           latitudelongitudeangle.cpp \
           linearinterpolator.cpp \
           mathsupport.cpp \
           mergesupportedlistcommand.cpp \
           modifycommanddialog.cpp \
           modifycommandwidget.cpp \
           mouseeventcontroller.cpp \
           multicelloperabletableview.cpp \
           networksetting.cpp \
           opacitycontainer.cpp \
           orderedvalueselectdialog.cpp \
           primitiveview.cpp \
           pythonutil.cpp \
           qfontcontainer.cpp \
           qfontdisplaylabel.cpp \
           qfonteditwidget.cpp \
           qimagedisplaywidget.cpp \
           qlabelwithclickedsignal.cpp \
           qmdiareawithscrollbehaviormodification.cpp \
           qpointfcompare.cpp \
           qpointfcontainer.cpp \
           qspinboxwithfocussignals.cpp \
           qtimezoneselectcombobox.cpp \
           qttool.cpp \
           qundocommandhelper.cpp \
           qwidgetcontainer.cpp \
           rectregion.cpp \
           slowmotionspeededitdialog.cpp \
           splineinterpolator.cpp \
           standarderrormessages.cpp \
           stringcontainer.cpp \
           stringtool.cpp \
           threadwithprogressinfo.cpp \
           tpoexporter.cpp \
           valuerangecontainer.cpp \
           valueselectdialog.cpp \
           versionnumber.cpp \
           windowgeometrycontainer.cpp \
           windowsizeeditwidget.cpp \
           windowsizestandardsetting.cpp \
           windowsizestandardsettingeditdialog.cpp \
           xmlattributecontainer.cpp \
           xmlsupport.cpp \
           zdepthrange.cpp \
           ziparchive.cpp
TRANSLATIONS += languages/iricMisc_ar_EG.ts \
                languages/iricMisc_bg_BG.ts \
                languages/iricMisc_bs_BA.ts \
                languages/iricMisc_ca_ES.ts \
                languages/iricMisc_cs_CZ.ts \
                languages/iricMisc_da_DK.ts \
                languages/iricMisc_de_DE.ts \
                languages/iricMisc_el_GR.ts \
                languages/iricMisc_es_ES.ts \
                languages/iricMisc_et_EE.ts \
                languages/iricMisc_eu_ES.ts \
                languages/iricMisc_fi_FI.ts \
                languages/iricMisc_fr_FR.ts \
                languages/iricMisc_gl_ES.ts \
                languages/iricMisc_hi_IN.ts \
                languages/iricMisc_hu_HU.ts \
                languages/iricMisc_id_ID.ts \
                languages/iricMisc_is_IS.ts \
                languages/iricMisc_it_IT.ts \
                languages/iricMisc_ja_JP.ts \
                languages/iricMisc_ko_KR.ts \
                languages/iricMisc_ky_KG.ts \
                languages/iricMisc_lt_LT.ts \
                languages/iricMisc_lv_LV.ts \
                languages/iricMisc_nb_NO.ts \
                languages/iricMisc_nl_NL.ts \
                languages/iricMisc_pl_PL.ts \
                languages/iricMisc_pt_BR.ts \
                languages/iricMisc_pt_PT.ts \
                languages/iricMisc_ro_RO.ts \
                languages/iricMisc_ru_RU.ts \
                languages/iricMisc_sl_SI.ts \
                languages/iricMisc_sv_SE.ts \
                languages/iricMisc_th_TH.ts \
                languages/iricMisc_tr_TR.ts \
                languages/iricMisc_uk_UA.ts \
                languages/iricMisc_vi_VN.ts \
                languages/iricMisc_zh_CN.ts \
                languages/iricMisc_zh_TW.ts
