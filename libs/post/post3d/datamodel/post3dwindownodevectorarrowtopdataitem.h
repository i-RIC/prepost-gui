#ifndef POST3DWINDOWNODEVECTORARROWTOPDATAITEM_H
#define POST3DWINDOWNODEVECTORARROWTOPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <QMap>
#include <QString>

class Post3dWindowNodeVectorArrowGroupDataItem;
class Post3dWindowZoneDataItem;
class SolverDefinitionGridType;

class QAction;

class Post3dWindowNodeVectorArrowTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowNodeVectorArrowTopDataItem(Post3dWindowDataItem* parent);

	double zScale() const;
	void innerUpdateZScale(double scale) override;
	void update();

	Post3dWindowZoneDataItem* zoneDataItem() const;

private:
	void addCustomMenuItems(QMenu* menu) override;

	QDialog* addDialog(QWidget* p) override;
	void handleAddDialogAccepted(QDialog* propDialog) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post3dWindowNodeVectorArrowGroupDataItem* buildItem(const std::string& name);

	double m_zScale;
};

#endif // POST3DWINDOWNODEVECTORARROWTOPDATAITEM_H
