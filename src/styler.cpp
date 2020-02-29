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
	// Set style
	if (style == FusionDark) {
		qApp->setStyle("Fusion");
		qApp->setPalette(darkPalette_);
	} else if (style == FusionLight) {
		qApp->setStyle("Fusion");
		qApp->setPalette(qApp->style()->standardPalette());
	} else if (style == OldschoolDark) {
		qApp->setStyle("Windows");
		qApp->setPalette(darkPalette_);
	} else if (style == OldschoolLight) {
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
		styles.append(style2string(FusionDark));
		styles.append(style2string(FusionLight));
	}
	if (available.contains("Windows")) {
		styles.append(style2string(OldschoolDark));
		styles.append(style2string(OldschoolLight));
	}
	return styles;
}

Style Styler::string2style(const QString &str) const
{
	// Get style from string
	if (str == "Fusion Dark") return FusionDark;
	if (str == "Fusion Light") return FusionLight;
	if (str == "Oldschool Dark") return OldschoolDark;
	if (str == "Oldschool Light") return OldschoolLight;
	else return Style::NoStyle;
}

QString Styler::style2string(const Style style) const
{
	// Get string from style
	switch (style) {
	case FusionDark: return "Fusion Dark";
	case FusionLight: return "Fusion Light";
	case OldschoolDark: return "Oldschool Dark";
	case OldschoolLight: return "Oldschool Light";
	default: return "";
	}
}
