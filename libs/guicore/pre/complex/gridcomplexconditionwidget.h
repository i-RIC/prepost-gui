#ifndef GRIDCOMPLEXCONDITIONWIDGET_H
#define GRIDCOMPLEXCONDITIONWIDGET_H

#include "../../guicore_global.h"

#include "gridcomplexconditiongroup.h"

#include <QWidget>

class QAbstractButton;
class SolverDefinition;
class InputConditionContainerSet;
class QDomElement;
class QLocale;

namespace Ui
{
	class GridComplexConditionWidget;
}

class InputConditionWidgetSet;
class iRICMainWindowI;
class PreProcessorGridRelatedComplexConditionDataItem;

class GUICOREDLL_EXPORT GridComplexConditionWidget : public QWidget
{
	Q_OBJECT

public:
	GridComplexConditionWidget(QWidget* parent = nullptr);
	~GridComplexConditionWidget();

	void setup(SolverDefinition* def, const QDomElement& elem);

	QString caption() const;
	void setCaption(const QString& caption);

	QColor color() const;
	void setColor(const QColor& color);

	bool isDefault() const;
	void setIsDefault(bool def);

	GridComplexConditionGroup* group();
	void setGroup(GridComplexConditionGroup* group);

signals:
	void captionChanged(const QString& caption);
	void defaultChecked(bool checked);

private slots:
	void handleDefaultCheck(bool checked);

private:
	void setWidget(QWidget* w);

	Ui::GridComplexConditionWidget* ui;

	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/gridcomplexconditionwidget_impl.h"
#endif // _DEBUG

#endif // GRIDCOMPLEXCONDITIONWIDGET_H
