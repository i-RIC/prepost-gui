#ifndef POST2DBIRDEYEWINDOWCELLSCALARGROUPTOPDATAITEM_H
#define POST2DBIRDEYEWINDOWCELLSCALARGROUPTOPDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"
#include <postbase/post2dwindowcontoursetting.h>

#include <QMap>

class Post2dBirdEyeWindowCellScalarGroupDataItem;

class Post2dBirdEyeWindowCellScalarGroupTopDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT
public:
	Post2dBirdEyeWindowCellScalarGroupTopDataItem(Post2dBirdEyeWindowDataItem* parent);
	~Post2dBirdEyeWindowCellScalarGroupTopDataItem();

	void update();

	bool nextScalarBarSetting(ScalarBarSetting& scalarBarSetting);

protected:
	void addCustomMenuItems(QMenu* menu) override;
	QDialog* addDialog(QWidget* parent) override;
	void handleAddDialogAccepted(QDialog* addDialog) override;
	void innerUpdateZScale(double scale) override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post2dWindowContourSetting m_setting;
	QMap<std::string, QString> m_colorbarTitleMap;
	std::map<std::string, Post2dBirdEyeWindowCellScalarGroupDataItem*> m_scalarmap; // only used by ctor and doLoadFromProjectMainFile
	double m_zScale;

	class SetSettingCommand;

	friend class Post2dBirdEyeWindowCellScalarGroupDataItem;

	class CreateCommand;
};

#endif // POST2DBIRDEYEWINDOWCELLSCALARGROUPTOPDATAITEM_H
