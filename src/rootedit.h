// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Föehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef ROOTEDIT_H
#define ROOTEDIT_H

#include <QLineEdit>
#include "parameters.h"

class RootEdit : public QLineEdit
{
	Q_OBJECT

public:
	RootEdit(QWidget *parent = nullptr);
	complex root() const;
	void setRoot(complex root);

private slots:
	void on_editingFinished();

signals:
	void rootChanged();

private:
	complex root_;
};

#endif // ROOTEDIT_H
