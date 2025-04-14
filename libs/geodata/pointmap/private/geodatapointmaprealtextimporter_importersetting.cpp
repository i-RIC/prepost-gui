#include "geodatapointmaprealtextimporter_importersetting.h"
#include "geodatapointmaprealtextimporter_lineparser.h"

#include <QTextCodec>

GeoDataPointmapRealTextImporter::ImporterSetting::ImporterSetting() :
	GeoDataImporterSetting {},
	codecName {"codecName"},
	csName {"csName"},
	delimiterComma {"delimiterComma", false},
	delimiterTab {"delimiterTab", false},
	delimiterSpace {"delimiterSpace", false},
	delimiterColon {"delimiterColon", false},
	delimiterSemicolon {"delimiterSemicolon", false},
	delimiterOther {"delimiterOther", false},
	otherDelimiter {"otherDelimiter"},
	quoteCharacter {"quoteCharacter"},
	escapeCharacter {"escapeCharacter"},
	headerLines {"headerLines", 0},
	fieldX {"fieldX", 0},
	fieldY {"fieldY", 0},
	fieldValue {"fieldV", 0},
	skipRate {"skipRate", 1}
{}

GeoDataPointmapRealTextImporter::LineParser* GeoDataPointmapRealTextImporter::ImporterSetting::buildParser()
{
	auto parser = new LineParser();

	auto codec = QTextCodec::codecForName(codecName.value().toLatin1());
	parser->setTextCodec(codec);

	std::vector<QChar> delimiters;
	if (delimiterComma.value()) {
		delimiters.push_back(',');
	}
	if (delimiterTab.value()) {
		delimiters.push_back('\t');
	}
	if (delimiterSpace.value()) {
		delimiters.push_back(' ');
	}
	if (delimiterColon.value()) {
		delimiters.push_back(':');
	}
	if (delimiterSemicolon.value()) {
		delimiters.push_back(';');
	}
	if (delimiterOther.value()) {
		auto other = otherDelimiter.value();
		for (int i = 0; i < other.size(); ++i) {
			delimiters.push_back(other.at(i));
		}
	}
	parser->setDelimiters(delimiters);

	std::vector<QChar> quotes;
	auto quoteStr = quoteCharacter.value();
	for (int i = 0; i < quoteStr.size(); ++i) {
		quotes.push_back(quoteStr.at(i));
	}
	parser->setQuoteChars(quotes);

	QChar escape;
	auto escapeStr = escapeCharacter.value();
	if (escapeStr.size() > 0) {
		escape = escapeStr.at(0);
	}
	parser->setEscapeChar(escape);

	parser->setHeaderLines(headerLines);
	parser->setXColumn(fieldX);
	parser->setYColumn(fieldY);
	parser->setValueColumn(fieldValue);
	parser->setSkipRate(skipRate);

	return parser;
}

void GeoDataPointmapRealTextImporter::ImporterSetting::doLoadFromProjectMainFile(const QDomNode& node)
{
	codecName.load(node);
	csName.load(node);
	delimiterComma.load(node);
	delimiterTab.load(node);
	delimiterSpace.load(node);
	delimiterColon.load(node);
	delimiterSemicolon.load(node);
	delimiterOther.load(node);
	otherDelimiter.load(node);
	quoteCharacter.load(node);
	escapeCharacter.load(node);
	headerLines.load(node);
	fieldX.load(node);
	fieldY.load(node);
	fieldValue.load(node);
	skipRate.load(node);
}

void GeoDataPointmapRealTextImporter::ImporterSetting::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	codecName.save(writer);
	csName.save(writer);
	delimiterComma.save(writer);
	delimiterTab.save(writer);
	delimiterSpace.save(writer);
	delimiterColon.save(writer);
	delimiterSemicolon.save(writer);
	delimiterOther.save(writer);
	otherDelimiter.save(writer);
	quoteCharacter.save(writer);
	escapeCharacter.save(writer);
	headerLines.save(writer);
	fieldX.save(writer);
	fieldY.save(writer);
	fieldValue.save(writer);
	skipRate.save(writer);
}

std::vector<GeoDataImporterSetting::Item> GeoDataPointmapRealTextImporter::ImporterSetting::customItems() const
{
	std::vector<GeoDataImporterSetting::Item> ret;
	QStringList delimiters;
	if (delimiterComma) {
		delimiters.push_back(tr("Comma"));
	}
	if (delimiterTab) {
		delimiters.push_back(tr("Tab"));
	}
	if (delimiterSpace) {
		delimiters.push_back(tr("Space"));
	}
	if (delimiterColon) {
		delimiters.push_back(tr("Colon"));
	}
	if (delimiterSemicolon) {
		delimiters.push_back(tr("Semicolon"));
	}
	if (delimiterOther) {
		delimiters.push_back(QString("\"%1\"").arg(otherDelimiter));
	}

	auto codec = QTextCodec::codecForName(codecName.value().toLatin1());
	ret.push_back(Item({tr("Encoding"), codec->name()}));
	ret.push_back(Item({tr("Coordinate System"), csName}));
	ret.push_back(Item({tr("Delimiters"), delimiters.join(", ")}));
	ret.push_back(Item({tr("Quote character"), quoteCharacter}));
	ret.push_back(Item({tr("Escape character"), escapeCharacter}));
	ret.push_back(Item({tr("Header lines to ignore"), QString("%1").arg(headerLines.value())}));
	ret.push_back(Item({tr("X field"), tr("Column %1").arg(fieldX.value() + 1)}));
	ret.push_back(Item({tr("Y field"), tr("Column %1").arg(fieldY.value() + 1)}));
	ret.push_back(Item({tr("Value field"), tr("Column %1").arg(fieldValue.value() + 1)}));
	ret.push_back(Item({tr("Skip rate"), QString("%1").arg(skipRate.value())}));

	return ret;
}
