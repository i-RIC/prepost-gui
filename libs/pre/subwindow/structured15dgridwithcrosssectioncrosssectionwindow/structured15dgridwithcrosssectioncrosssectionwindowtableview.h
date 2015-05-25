#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOWTABLEVIEW_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOWTABLEVIEW_H

#include <QTableView>

class Structured15DGridWithCrossSectionCrossSectionWindowTableView : public QTableView
{
	Q_OBJECT
public:
	explicit Structured15DGridWithCrossSectionCrossSectionWindowTableView(QWidget* parent = nullptr);
protected:
	void resizeEvent(QResizeEvent* event) override;
private:
	const static int columnMargin = 0;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOWTABLEVIEW_H
