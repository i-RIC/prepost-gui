#ifndef POST2DWINDOWSCALARGROUPTOPDATAITEM_H
#define POST2DWINDOWSCALARGROUPTOPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <QMap>

class ScalarBarSetting;

class Post2dWindowScalarGroupTopDataItem : public Post2dWindowDataItem
{
    Q_OBJECT

public:
    Post2dWindowScalarGroupTopDataItem(GraphicsWindowDataItem* parent);
    Post2dWindowScalarGroupTopDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent);
    Post2dWindowScalarGroupTopDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent);

    virtual ~Post2dWindowScalarGroupTopDataItem();

    bool nextScalarBarSetting(ScalarBarSetting& scalarBarSetting);

    QMap<std::string, QString>& colorbarTitleMap();

private:
    // for scalar bar
    QMap<std::string, QString> m_colorbarTitleMap;

};

#endif // POST2DWINDOWSCALARGROUPTOPDATAITEM_H
