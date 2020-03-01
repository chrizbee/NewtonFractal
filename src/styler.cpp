// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "styler.h"
#include <QFile>
#include <QApplication>
#include <QStyleFactory>
#include <QFontDatabase>

Styler::Styler()
{
	// Initialize font
	int id = QFontDatabase::addApplicationFont("://resources/fonts/fira.ttf");
	if (id != -1) {
		QString family = QFontDatabase::applicationFontFamilies(id).at(0);
		font_ = QFont(family, 10);
	} else font_ = QFont("Consolas", 10);
	qApp->setFont(font_);

	// Initialize dark palette
	darkPalette_.setColor(QPalette::Window,QColor(53,53,53));
	darkPalette_.setColor(QPalette::WindowText,Qt::white);
	darkPalette_.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
	darkPalette_.setColor(QPalette::Base,QColor(42,42,42));
	darkPalette_.setColor(QPalette::AlternateBase,QColor(66,66,66));
	darkPalette_.setColor(QPalette::ToolTipBase,Qt::white);
	darkPalette_.setColor(QPalette::ToolTipText,Qt::white);
	darkPalette_.setColor(QPalette::Text,Qt::white);
	darkPalette_.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
	darkPalette_.setColor(QPalette::Dark,QColor(35,35,35));
	darkPalette_.setColor(QPalette::Shadow,QColor(20,20,20));
	darkPalette_.setColor(QPalette::Button,QColor(53,53,53));
	darkPalette_.setColor(QPalette::ButtonText,Qt::white);
	darkPalette_.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
	darkPalette_.setColor(QPalette::BrightText,Qt::red);
	darkPalette_.setColor(QPalette::Link,QColor(42,130,218));
	darkPalette_.setColor(QPalette::Highlight,QColor(42,130,218));
	darkPalette_.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
	darkPalette_.setColor(QPalette::HighlightedText,Qt::white);
	darkPalette_.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));
}

void Styler::setStyle(const Style style)
{
	// Static style at first call
	static const QString systemStyle = qApp->style()->objectName();

	// Set style
	if (style == SYSTEM) {
		qApp->setStyle(systemStyle);
		qApp->setPalette(qApp->style()->standardPalette());
	} else if (style == FUSIONDARK) {
		qApp->setStyle("Fusion");
		qApp->setPalette(darkPalette_);
	} else if (style == FUSIONLIGHT) {
		qApp->setStyle("Fusion");
		qApp->setPalette(qApp->style()->standardPalette());
	} else if (style == OLDSCHOOLDARK) {
		qApp->setStyle("Windows");
		qApp->setPalette(darkPalette_);
	} else if (style == OLDSCHOOLLIGHT) {
		qApp->setStyle("Windows");
		qApp->setPalette(qApp->style()->standardPalette());
	}

	// Set font bcs it resets by setting the style
	qApp->setFont(font_);
}

void Styler::setStyle(const QString &style)
{
	// Set style
	setStyle(string2style(style));
}

QStringList Styler::availableStyles() const
{
	// List available styles
	QStringList styles;
	QStringList available = QStyleFactory::keys();
	if (available.contains("Fusion")) {
		styles.append(style2string(FUSIONDARK));
		styles.append(style2string(FUSIONLIGHT));
	}
	if (available.contains("Windows")) {
		styles.append(style2string(OLDSCHOOLDARK));
		styles.append(style2string(OLDSCHOOLLIGHT));
	}
	styles.append(style2string(SYSTEM));
	return styles;
}

Style Styler::string2style(const QString &str) const
{
	// Get style from string
	if (str == "System") return SYSTEM;
	if (str == "Fusion Dark") return FUSIONDARK;
	if (str == "Fusion Light") return FUSIONLIGHT;
	if (str == "Oldschool Dark") return OLDSCHOOLDARK;
	if (str == "Oldschool Light") return OLDSCHOOLLIGHT;
	else return NOSTYLE;
}

QString Styler::style2string(const Style style) const
{
	// Get string from style
	switch (style) {
	case SYSTEM: return "System";
	case FUSIONDARK: return "Fusion Dark";
	case FUSIONLIGHT: return "Fusion Light";
	case OLDSCHOOLDARK: return "Oldschool Dark";
	case OLDSCHOOLLIGHT: return "Oldschool Light";
	default: return "";
	}
}
