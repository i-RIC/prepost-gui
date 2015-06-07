#ifndef POSTBASESELECTINGDIALOG_H
#define POSTBASESELECTINGDIALOG_H

#include "../guicore_global.h"
#include <QDialog>
#include "postsolutioninfo.h"

namespace Ui
{
	class PostBaseSelectingDialog;
}

class GUICOREDLL_EXPORT PostBaseSelectingDialog : public QDialog
{
	Q_OBJECT

public:
	enum BaseDimension {
		Dimension1 = 1,
		Dimension2 = 2,
		Dimension3 = 4
	};
	explicit PostBaseSelectingDialog(QWidget* parent = nullptr);
	~PostBaseSelectingDialog();
	void setAvailableDimensions(int dimensions);
	BaseDimension selectedDimension();
	PostSolutionInfo::Dimension selectedPostDimension();
	static PostSolutionInfo::Dimension getPostDimension(BaseDimension dim);

private:
	Ui::PostBaseSelectingDialog* ui;
};

#endif // POSTBASESELECTINGDIALOG_H
