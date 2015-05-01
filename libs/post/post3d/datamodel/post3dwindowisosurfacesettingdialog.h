#ifndef POST3DWINDOWISOSURFACESETTINGDIALOG_H
#define POST3DWINDOWISOSURFACESETTINGDIALOG_H

#include <guibase/contoursettingwidget.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include "post3dwindownodescalargroupdataitem.h"

#include <QDialog>
#include <QList>

namespace Ui
{
	class Post3dWindowIsosurfaceSettingDialog;
}

class PostZoneDataContainer;
class LookupTableContainer;
class Post3dWindowGridTypeDataItem;

class Post3dWindowIsosurfaceSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post3dWindowIsosurfaceSettingDialog(QWidget* parent = nullptr);
	~Post3dWindowIsosurfaceSettingDialog();

	void setZoneData(PostZoneDataContainer* zoneData);
	void setEnabled(bool enabled);

	void setCurrentSolution(QString sol);
	void setGridTypeDataItem(Post3dWindowGridTypeDataItem* item);
	void setFullRange(bool full);
	void setRange(StructuredGridRegion::Range3d range);
	void setIsoValue(double val);

	bool enabled();

	void setColor(const QColor& color);
	const QColor color() const;

	QString currentSolution();
	bool fullRange();
	StructuredGridRegion::Range3d range();
	double isoValue();

public slots:
	void accept();

private slots:
	void setEnableCheckBox(bool);
	void solutionChanged(int index);
	void fullRangeChanged(bool full);
	void iMinChanged(int min);
	void iMaxChanged(int max);
	void jMinChanged(int min);
	void jMaxChanged(int max);
	void kMinChanged(int min);
	void kMaxChanged(int max);

private:
	QList<QString> m_solutions;
	Ui::Post3dWindowIsosurfaceSettingDialog* ui;
	Post3dWindowGridTypeDataItem* m_gridTypeDataItem;

};

#endif // POST3DWINDOWISOSURFACESETTINGDIALOG_H
