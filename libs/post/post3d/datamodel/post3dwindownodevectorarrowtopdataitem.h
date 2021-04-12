#ifndef POST3DWINDOWNODEVECTORARROWTOPDATAITEM_H
#define POST3DWINDOWNODEVECTORARROWTOPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <QMap>
#include <QString>

class QAction;

class Post3dWindowNodeVectorArrowTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowNodeVectorArrowTopDataItem(Post3dWindowDataItem* parent);

	double zScale() const;
	void innerUpdateZScale(double scale) override;
	void update();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void addCustomMenuItems(QMenu* menu) override;

	QDialog* addDialog(QWidget* p) override;
	void handleAddDialogAccepted(QDialog* propDialog) override;

	QMap<std::string, QString> m_colorBarTitleMap;
	double m_zScale;

	class CreateCommand;;
};

#endif // POST3DWINDOWNODEVECTORARROWTOPDATAITEM_H
