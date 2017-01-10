#ifndef POST3DWINDOWCONTOURGROUPTOPDATAITEM_H
#define POST3DWINDOWCONTOURGROUPTOPDATAITEM_H

#include "../post3dwindowdataitem.h"

class Post3dWindowContourGroupTopDataItem : public Post3dWindowDataItem
{
public:
	Post3dWindowContourGroupTopDataItem(Post3dWindowDataItem* p);
	~Post3dWindowContourGroupTopDataItem();

	void innerUpdateZScale(double scale) override;
	void update();

protected:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;
	QDialog* propertyDialog(QWidget* p) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

private:
	QMap<std::string, QString> m_colorBarTitleMap;
	double m_zScale;

	friend class Post3dWindowContourGroupDataItem;

	class CreateCommand;
};

#endif // POST3DWINDOWCONTOURGROUPTOPDATAITEM_H
