#ifndef STYLER_H
#define STYLER_H

#include <QFont>
#include <QStyle>
#include <QPalette>
#include <QStringList>

enum Style {
	SYSTEM,
	FUSIONDARK,
	FUSIONLIGHT,
	OLDSCHOOLDARK,
	OLDSCHOOLLIGHT,
	NOSTYLE
};

class Styler
{
public:
	Styler();
	void setStyle(const Style style);
	void setStyle(const QString &style);
	QStringList availableStyles() const;
	Style string2style(const QString &str) const;
	QString style2string(const Style style) const;

private:
	QFont font_;
	QPalette darkPalette_;
};

#endif // STYLER_H
