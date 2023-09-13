#ifndef COPYCAMERASETTINGDIALOG_H
#define COPYCAMERASETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class CopyCameraSettingDialog;
}

class WindowWithVtkGraphicsViewI;

class CopyCameraSettingDialog : public QDialog
{
	Q_OBJECT

public:
	CopyCameraSettingDialog(QMdiArea* area, QWidget *parent = nullptr);
	~CopyCameraSettingDialog();

public slots:
	void accept() override;

private slots:
	void checkAll();
	void uncheckAll();
	void updateTargets();

private:
	class Setting {
	public:
		enum class Type {TwoDimensional, ThreeDimensional};
		bool operator==(const Setting& s) const {return window == s.window;}

		Type type;
		QString name;
		WindowWithVtkGraphicsViewI* window;
	};
	void setupSources();
	void setupTargets(const Setting& setting);

	std::vector<Setting> m_sources;
	std::vector<Setting> m_targets;

	QMdiArea* m_mdiArea;
	Ui::CopyCameraSettingDialog *ui;
};

#endif // COPYCAMERASETTINGDIALOG_H
