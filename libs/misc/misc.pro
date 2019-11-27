TARGET = iricMisc
TEMPLATE = lib

DEFINES += MISC_LIBRARY

QT += network widgets xml

include( ../../paths.pri )

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += anms.h \
           boolcontainer.h \
           colorcontainer.h \
           compositecontainer.h \
           doublecontainer.h \
           doublemappingsetting.h \
           dvector.h \
           edge.h \
           enumcontainert.h \
           errormessage.h \
           filesystemfunction.h \
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
           misc_global.h \
           multicelloperabletableview.h \
           networksetting.h \
           numericvaluecontainert.h \
           opacitycontainer.h \
           primitiveview.h \
           qpointfcontainer.h \
           qttool.h \
           simplevaluecontainert.h \
           slowmotionspeededitdialog.h \
           splineinterpolator.h \
           standarderrormessages.h \
           stringcontainer.h \
           stringtool.h \
           threadwithprogressinfo.h \
           valuearraycontainert.h \
           valuechangert.h \
           valuerestorert.h \
           versionnumber.h \
           windowgeometrycontainer.h \
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
           private/simplevaluecontainert_detail.h \
           private/valuearraycontainert_detail.h \
           private/valuechangert_detail.h \
           private/valuerestorert_detail.h \
           private/versionnumber_impl.h \
           private/xmlattributecontainer_impl.h \
           private/zdepthrange_impl.h
FORMS += informationdialog.ui primitiveview.ui slowmotionspeededitdialog.ui
SOURCES += anms.cpp \
           boolcontainer.cpp \
           colorcontainer.cpp \
           compositecontainer.cpp \
           doublecontainer.cpp \
           errormessage.cpp \
           filesystemfunction.cpp \
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
           multicelloperabletableview.cpp \
           networksetting.cpp \
           opacitycontainer.cpp \
           primitiveview.cpp \
           qpointfcontainer.cpp \
           qttool.cpp \
           slowmotionspeededitdialog.cpp \
           splineinterpolator.cpp \
           standarderrormessages.cpp \
           stringcontainer.cpp \
           stringtool.cpp \
           threadwithprogressinfo.cpp \
           versionnumber.cpp \
           windowgeometrycontainer.cpp \
           xmlattributecontainer.cpp \
           xmlsupport.cpp \
           zdepthrange.cpp \
           ziparchive.cpp
TRANSLATIONS += languages/iricMisc_es_ES.ts \
                languages/iricMisc_fr_FR.ts \
                languages/iricMisc_id_ID.ts \
                languages/iricMisc_ja_JP.ts \
                languages/iricMisc_ko_KR.ts \
                languages/iricMisc_ru_RU.ts \
                languages/iricMisc_th_TH.ts \
                languages/iricMisc_vi_VN.ts \
                languages/iricMisc_zh_CN.ts
