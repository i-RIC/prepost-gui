#ifndef REALNUMBERDISPLAYLABEL_H
#define REALNUMBERDISPLAYLABEL_H

#include "../guibase_global.h"

#include <QLabel>

/// Label to show real number.
class GUIBASEDLL_EXPORT RealNumberDisplayLabel : public QLabel
{
	Q_OBJECT

public:
	enum ClearMode {
		cmEmpty,    ///< Display nothing
		cmLines,    ///< Display lines, like "-----"
	};
	explicit RealNumberDisplayLabel(QWidget* parent = nullptr);

	ClearMode clearMode() const;
	void setClearMode(ClearMode cm);

public slots:
	void setValue(double value);
	void clear();

private:
	ClearMode m_clearMode;
};

#endif // REALNUMBERDISPLAYLABEL_H
