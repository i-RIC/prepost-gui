#ifndef POST2DWINDOWCONTOURSETTINGDIALOG_H
#define POST2DWINDOWCONTOURSETTINGDIALOG_H

#include "../post2d_global.h"
#include "post2dwindownodescalargroupdataitem.h"

#include <guibase/contoursettingwidget.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>

#include <QDialog>
#include <QList>

namespace Ui
{
	class Post2dWindowContourSettingDialog;
}

class PostZoneDataContainer;
class LookupTableContainer;
class Post2dWindowGridTypeDataItemInterface;

class POST2D_EXPORT Post2dWindowContourSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post2dWindowContourSettingDialog(QWidget* parent = nullptr);
	~Post2dWindowContourSettingDialog();
	void setZoneData(PostZoneDataContainer* zoneData);
	void setGridTypeDataItem(Post2dWindowGridTypeDataItemInterface* item);
	void disableActive();
	void hideOpacity();

	void setSetting(const Post2dWindowNodeScalarGroupDataItem::Setting& setting);
	void setColorBarTitleMap(const QMap<QString, QString>& titlemap);

	Post2dWindowNodeScalarGroupDataItem::Setting setting() const;
	const LookupTableContainer& lookupTable();
	QString scalarBarTitle();

public slots:
	void accept() override;

private slots:
	void solutionChanged(int index);
	void showRegionDialog();
	void showScalarBarDialog();

private:
	QString currentSolution() const;

	int m_gridDims[3];
	QList<QString> m_solutions;
	Post2dWindowGridTypeDataItemInterface* m_gridTypeDataItem;
	LookupTableContainer m_lookupTable;
	bool m_activeAvailable;
	bool m_unstructured;

	Post2dWindowNodeScalarGroupDataItem::Setting m_setting;
	QMap<QString, QString> m_colorBarTitleMap;
	Ui::Post2dWindowContourSettingDialog* ui;
};

#endif // POST2DWINDOWCONTOURSETTINGDIALOG_H
