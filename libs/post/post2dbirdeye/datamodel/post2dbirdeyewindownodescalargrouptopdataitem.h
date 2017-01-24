#ifndef POST2DBIRDEYEWINDOWNODESCALARGROUPTOPDATAITEM_H
#define POST2DBIRDEYEWINDOWNODESCALARGROUPTOPDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"
#include <postbase/post2dwindowcontoursetting.h>

#include <QMap>

class Post2dBirdEyeWindowNodeScalarGroupTopDataItem : public Post2dBirdEyeWindowDataItem
{
public:
	Post2dBirdEyeWindowNodeScalarGroupTopDataItem(Post2dBirdEyeWindowDataItem* parent);
	~Post2dBirdEyeWindowNodeScalarGroupTopDataItem();

	void update();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

protected:
	void innerUpdateZScale(double scale) override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post2dWindowContourSetting m_setting;
	QMap<std::string, QString> m_colorbarTitleMap;
	double m_zScale;

	class SetSettingCommand;

	friend class Post2dBirdEyeWindowNodeScalarGroupDataItem;

	class CreateCommand;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARGROUPTOPDATAITEM_H
