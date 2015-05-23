#ifndef CONTOURSETTINGWIDGET_H
#define CONTOURSETTINGWIDGET_H

#include "guibase_global.h"
#include <QWidget>

namespace Ui
{
	class ContourSettingWidget;
}

/**
 * @brief Widget to select contour type
 *
 * Contour type can be selected from one of the followings:
 * - Isolines
 * - Contour figure
 * - Color fringe
 *
 * In some special cases (like measured values), the following
 * can be selected too.
 * - Point
 *
 */
class GUIBASEDLL_EXPORT ContourSettingWidget : public QWidget
{
	Q_OBJECT

public:
	/// Contour type
	enum Contour {
		Points,        ///< No contour. Points are shown where data was observed
		Isolines,      ///< Isolines
		ContourFigure, ///< Contour figure
		ColorFringe    ///< Color fringe
	};
	ContourSettingWidget(QWidget* parent = nullptr);
	~ContourSettingWidget();
	/// Set the contour type setting
	void setContour(Contour contour);
	/// The contour type setting
	Contour contour();
	/// Hide "Points" radio button
	void hidePoints();
	/// Set whether no polydata exists. If nopolydata is true, Users can select only "Points".
	void setNoPolyData(bool nopolydata);

signals:
	void contourChanged();
	void contourChanged(ContourSettingWidget::Contour c);

private slots:
	void handleContourChange();

private:
	Ui::ContourSettingWidget* ui;
};

#endif // CONTOURSETTINGWIDGET_H
