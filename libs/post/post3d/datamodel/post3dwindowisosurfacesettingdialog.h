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

	void setCurrentSolution(const std::string& sol);
	void setGridTypeDataItem(Post3dWindowGridTypeDataItem* item);
	void setFullRange(bool full);
	void setRange(StructuredGridRegion::Range3d range);
	void setIsoValue(double val);

	bool enabled();

	void setColor(const QColor& color);
	const QColor color() const;

	std::string currentSolution() const;
	bool fullRange() const;
	StructuredGridRegion::Range3d range() const;
	double isoValue() const;

public slots:
	void accept() override;

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
	QList<std::string> m_solutions;
	Ui::Post3dWindowIsosurfaceSettingDialog* ui;
	Post3dWindowGridTypeDataItem* m_gridTypeDataItem;

};

#endif // POST3DWINDOWISOSURFACESETTINGDIALOG_H
