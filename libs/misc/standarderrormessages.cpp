#include "standarderrormessages.h"

QString StandardErrorMessages::fileOpenError(const QString& filename)
{
	return tr("Opening file %1 failed.").arg(filename);
}

StandardErrorMessages::StandardErrorMessages()
{}
