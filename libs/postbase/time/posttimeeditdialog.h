#ifndef POSTTIMEEDITDIALOG_H
#define POSTTIMEEDITDIALOG_H

#include "../postbase_global.h"
#include "posttimedataitem.h"
#include <QDialog>

namespace Ui
{
	class PostTimeEditDialog;
}

class POSTBASEDLL_EXPORT PostTimeEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostTimeEditDialog(QWidget* parent = nullptr);
	~PostTimeEditDialog();
	void setSetting(const PostTimeDataItem::Setting& setting);
	PostTimeDataItem::Setting setting() const;

private:
	Ui::PostTimeEditDialog* ui;
};

#endif // POSTTIMEEDITDIALOG_H
