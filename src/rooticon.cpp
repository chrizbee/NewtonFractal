// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "rooticon.h"
#include <QPixmap>
#include <QPainter>

RootIcon::RootIcon(QColor color, QWidget *parent) :
	QPushButton(parent)
{
	// Initialize label
	setMaximumSize(30, 30);
	setMinimumSize(30, 30);
	setFlat(true);
	setColor(color);
}

void RootIcon::setColor(QColor color)
{
	// Paint pixmap
	QPixmap icon(30, 30);
	icon.fill(Qt::transparent);
	QPainter painter(&icon);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(color);
	painter.setPen(color);
	painter.drawEllipse(0, 0, 30, 30);
	setIcon(QIcon(icon));
	setIconSize(QSize(18, 18));
}
