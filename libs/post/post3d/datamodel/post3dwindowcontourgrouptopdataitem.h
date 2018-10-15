#ifndef POST3DWINDOWCONTOURGROUPTOPDATAITEM_H
#define POST3DWINDOWCONTOURGROUPTOPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <QMap>

class Post3dWindowContourGroupTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowContourGroupTopDataItem(Post3dWindowDataItem* p);
	~Post3dWindowContourGroupTopDataItem();

	void innerUpdateZScale(double scale) override;
	void update();

protected:
	void addCustomMenuItems(QMenu* menu) override;
	QDialog* addDialog(QWidget* p) override;
	void handleAddDialogAccepted(QDialog* propDialog) override;
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

private:
	QMap<std::string, QString> m_colorBarTitleMap;
	double m_zScale;

	friend class Post3dWindowContourGroupDataItem;

	class CreateCommand;

	QAction* m_addAction;
};

#endif // POST3DWINDOWCONTOURGROUPTOPDATAITEM_H
