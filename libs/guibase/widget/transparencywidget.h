#ifndef TRANSPARENCYWIDGET_H
#define TRANSPARENCYWIDGET_H

#include "../guibase_global.h"

#include <QWidget>

namespace Ui
{
	class TransparencyWidget;
}

class OpacityContainer;

/// Widget to edit transparency with slider
class GUIBASEDLL_EXPORT TransparencyWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TransparencyWidget(QWidget* parent = nullptr);
	~TransparencyWidget();

	void setOpacity(const OpacityContainer& opacity);
	OpacityContainer opacity() const;

	void setOpacityPercent(const int opacityPercent);
	int opacityPercent() const;

signals:
	void updated(const OpacityContainer& opacity);

private:
	Ui::TransparencyWidget* ui;
};

#endif // TRANSPARENCYWIDGET_H
