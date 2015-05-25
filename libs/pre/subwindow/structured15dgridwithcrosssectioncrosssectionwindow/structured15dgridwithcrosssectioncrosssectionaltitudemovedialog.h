#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONALTITUDEMOVEDIALOG_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONALTITUDEMOVEDIALOG_H

#include <QDialog>

class QAbstractButton;
class Structured15DGridWithCrossSectionCrossSection;
class Structured15DGridWithCrossSectionCrossSectionWindow;
class PreProcessorNormal15DGridWithCrossSectionShapeDataItem;

namespace Ui
{
	class Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog;
}

class Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog : public QDialog
{
	Q_OBJECT
public:
	explicit Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog(Structured15DGridWithCrossSectionCrossSection* cross, int from, int to, Structured15DGridWithCrossSectionCrossSectionWindow* w, PreProcessorNormal15DGridWithCrossSectionShapeDataItem* item, QWidget* parent);
	~Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog();
public slots:
	void accept() override;
	void reject() override;
private slots:
	void hOffsetChange();
	void handleButtonClick(QAbstractButton* button);
private:
	Ui::Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog* ui;

	void setLimits();
	void doReset();
	void apply();
	void updateCrossSection();

	bool m_leftLimitSet;
	bool m_rightLimitSet;
	double m_leftLimit;
	double m_rightLimit;

	int m_from;
	int m_to;

	Structured15DGridWithCrossSectionCrossSectionWindow* m_window;
	Structured15DGridWithCrossSectionCrossSection* m_cross;
	PreProcessorNormal15DGridWithCrossSectionShapeDataItem* m_shapeItem;
	bool m_applyed;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONALTITUDEMOVEDIALOG_H
