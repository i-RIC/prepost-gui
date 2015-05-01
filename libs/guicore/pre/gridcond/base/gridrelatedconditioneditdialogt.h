#ifndef GRIDRELATEDCONDITIONEDITDIALOGT_H
#define GRIDRELATEDCONDITIONEDITDIALOGT_H

#include "gridrelatedconditioneditdialog.h"
#include "gridrelatedconditioncontainert.h"
#include "../../../solverdef/solverdefinitiongridrelatedconditiont.h"

/// Dialog to Edit Grid related condition value
template <class V>
class GridRelatedConditionEditDialogT : public GridRelatedConditionEditDialog
{
public:
	GridRelatedConditionEditDialogT(QWidget* parent, SolverDefinitionGridRelatedConditionT<V>* cond)
		: GridRelatedConditionEditDialog(parent, cond) {
		m_value = 0;
	}
	void setValue(V value) {
		m_value = value;
		m_valueCleared = false;
		m_valueSelected = true;
		setupDialog();
	}
	V value() {
		return m_value;
	}
	void setVariantValue(const QVariant& v) {
		SolverDefinitionGridRelatedConditionT<V>* cond = dynamic_cast<SolverDefinitionGridRelatedConditionT<V>* >(m_gridRelatedCondition);
		V tmpval = cond->fromVariant(v);
		setValue(tmpval);
	}

	QVariant variantValue() {
		return QVariant(m_value);
	}
	void scanAndSetDefault(GridRelatedConditionContainer* container, QVector<unsigned int>& indices) {
		GridRelatedConditionContainerT<V>* c = dynamic_cast<GridRelatedConditionContainerT<V>* >(container);
		bool same = true;
		V val;
		auto it = indices.begin();
		val = c->value(*it);
		++it;
		while (same && it != indices.end()) {
			same &= (val == c->value(*it));
			++it;
		}
		if (same) {
			setValue(val);
		} else {
			clearValue();
		}
	}

	void applyValue(GridRelatedConditionContainer* container, QVector<unsigned int>& indices) {
		if (! m_valueSelected) {return;}
		V val = value();
		GridRelatedConditionContainerT<V>* c = dynamic_cast<GridRelatedConditionContainerT<V>* >(container);
		for (auto it = indices.begin(); it != indices.end(); ++it) {
			c->setValue(*it, val);
		}
		c->dataArray()->Modified();
	}
protected:
	V m_value;
};

#endif // GRIDRELATEDCONDITIONEDITDIALOGT_H
