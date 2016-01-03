#ifndef POST2DGRIDREGIONSELECTDIALOG_H
#define POST2DGRIDREGIONSELECTDIALOG_H

#include "post2dwindowdataitem.h"
#include <guibase/structuredgridregion.h>
#include <QDialog>

namespace Ui
{
	class Post2dGridRegionSelectDialog;
}

class Post2dGridRegionSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post2dGridRegionSelectDialog(QWidget* parent = nullptr);
	~Post2dGridRegionSelectDialog();

	void disableActive();
	void hideCustom();
	void setGridSize(int isize, int jsize);

	StructuredGridRegion::RegionMode regionMode();
	void setRegionMode(StructuredGridRegion::RegionMode rm);

	StructuredGridRegion::Range2d region();
	void setRegion(const StructuredGridRegion::Range2d& region);

private:
	Ui::Post2dGridRegionSelectDialog* ui;
};

#endif // POST2DGRIDREGIONSELECTDIALOG_H
