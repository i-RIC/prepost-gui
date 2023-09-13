#ifndef WINDOWSIZEEDITWIDGET_H
#define WINDOWSIZEEDITWIDGET_H

#include "misc_global.h"

#include "windowsizestandardsetting.h"

#include <QWidget>

namespace Ui {
class WindowSizeEditWidget;
}

class MISCDLL_EXPORT WindowSizeEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit WindowSizeEditWidget(QWidget *parent = nullptr);
	~WindowSizeEditWidget();

	QSize size() const;
	void setSize(const QSize& size);

private:
	std::vector<WindowSizeStandardSetting> m_settings;
	Ui::WindowSizeEditWidget *ui;
};

#endif // WINDOWSIZEEDITWIDGET_H
