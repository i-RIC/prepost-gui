#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONALTITUDEMOVEDIALOG_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONALTITUDEMOVEDIALOG_H

#include <QDialog>

class v4Structured15dGridWithCrossSectionCrossSection;
class Structured15DGridWithCrossSectionCrossSectionWindow;
class PreProcessorNormal15DGridWithCrossSectionShapeDataItem;

class QAbstractButton;

namespace Ui
{
	class Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog;
}

class Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog(v4Structured15dGridWithCrossSectionCrossSection* cross, int from, int to, Structured15DGridWithCrossSectionCrossSectionWindow* w, PreProcessorNormal15DGridWithCrossSectionShapeDataItem* item, QWidget* parent);
	~Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog();

public slots:
	void accept() override;
	void reject() override;

private slots:
	void hOffsetChange();
	void handleButtonClick(QAbstractButton* button);

private:
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
	v4Structured15dGridWithCrossSectionCrossSection* m_cross;
	PreProcessorNormal15DGridWithCrossSectionShapeDataItem* m_shapeItem;
	bool m_applyed;

	Ui::Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog* ui;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONALTITUDEMOVEDIALOG_H
