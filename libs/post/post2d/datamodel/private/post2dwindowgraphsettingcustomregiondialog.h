#ifndef POST2DWINDOWGRAPHSETTINGCUSTOMREGIONDIALOG_H
#define POST2DWINDOWGRAPHSETTINGCUSTOMREGIONDIALOG_H

#include <QDialog>

#include <vector>

namespace Ui {
class Post2dWindowGraphSettingCustomRegionDialog;
}

class Post2dWindowGraphSettingCustomRegionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post2dWindowGraphSettingCustomRegionDialog(QWidget *parent = 0);
	~Post2dWindowGraphSettingCustomRegionDialog();

	void setIndexCount(int count);

	std::vector<int> setting() const;
	void setSetting(const std::vector<int>& setting);

private slots:
	void selectAll();
	void unselectAll();

private:
	Ui::Post2dWindowGraphSettingCustomRegionDialog *ui;
};

#endif // POST2DWINDOWGRAPHSETTINGCUSTOMREGIONDIALOG_H
