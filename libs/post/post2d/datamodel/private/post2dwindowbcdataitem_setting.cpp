#include "post2dwindowbcdataitem_setting.h"
#include <QColor>

Post2dWindowBCDataItem::Setting::Setting() :
    CompositeContainer {&opacity, &color, &showName, &pointSize},
    opacity {"opacity", 50},
    color {"color", Qt::white},
    showName {"showName", true},
    pointSize {"pointSize", 7}
{}

Post2dWindowBCDataItem::Setting::Setting(const Setting& setting) :
    Setting {}
{
    copyValue(setting);
}

Post2dWindowBCDataItem::Setting& Post2dWindowBCDataItem::Setting::operator=(const Setting& setting)
{
    copyValue(setting);
    return *this;
}

XmlAttributeContainer& Post2dWindowBCDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
    return operator=(dynamic_cast<const Setting&>(c));
}