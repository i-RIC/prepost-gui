#ifndef TRANSPARENCYWIDGET_H
#define TRANSPARENCYWIDGET_H

#include "guibase_global.h"

#include <QWidget>

namespace Ui
{
	class TransparencyWidget;
}

/// Widget to edit transparency with slider
class GUIBASEDLL_EXPORT TransparencyWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TransparencyWidget(QWidget* parent = nullptr);
	~TransparencyWidget();
	/// Set the opacity value (0 to 100)
	void setOpacity(int opacity);
	/// The opacity value (0 to 100)
	int opacity();

private slots:
	void handleCheck(bool checked);

private:
	Ui::TransparencyWidget* ui;
};

#endif // TRANSPARENCYWIDGET_H
