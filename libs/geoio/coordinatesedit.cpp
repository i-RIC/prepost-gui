#include "coordinatesedit.h"
#include "coordinateseditdialog.h"

void CoordinatesEdit::edit(const std::vector<QPointF>& coords, QWidget* parent,
						accept_callback acceptCallback,
						reject_callback rejectCallback,
						apply_callback applyCallback,
						const QString& dialogTitle)
{
	auto dialog = new CoordinatesEditDialog(parent);
	dialog->setCoordinates(coords);
	dialog->setAcceptCallback(acceptCallback);
	dialog->setRejectCallback(rejectCallback);
	dialog->setApplyCallback(applyCallback);
	dialog->setWindowTitle(dialogTitle);

	dialog->show();
}

std::vector<QPointF> CoordinatesEdit::applyOffset(const std::vector<QPointF>& coords, const QPointF& offset)
{
	std::vector<QPointF> ret;
	for (const auto& p : coords) {
		ret.push_back(QPointF(p.x() + offset.x(), p.y() + offset.y()));
	}
	return ret;
}
std::vector<QPointF> CoordinatesEdit::unapplyOffset(const std::vector<QPointF>& coords, const QPointF& offset)
{
	return applyOffset(coords, - offset);
}

CoordinatesEdit::CoordinatesEdit()
{}
