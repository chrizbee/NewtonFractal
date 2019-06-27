// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef ROOTICON_H
#define ROOTICON_H

#include <QPushButton>

class RootIcon : public QPushButton
{
	Q_OBJECT

public:
	RootIcon(QColor color, QWidget *parent = nullptr);
	void setColor(QColor color);
};

#endif // ROOTICON_H
