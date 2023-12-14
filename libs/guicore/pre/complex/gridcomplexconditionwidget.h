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

	QString caption() const;

	QColor color() const;

	bool isDefault() const;
	void hideWidgetsNotForCalculationCondition();

	GridComplexConditionGroup* group();
	void setGroup(GridComplexConditionGroup* group);

signals:
	void captionChanged(const QString& caption);
	void defaultChecked(bool checked);

public slots:
	void setCaption(const QString& caption);
	void setColor(const QColor& color);
	void setIsDefault(bool def);

private slots:
	void handleCaptionEdit(const QString caption);
	void handleColorEdit(const QColor& color);
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
