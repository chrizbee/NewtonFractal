// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef SIZEEDIT_H
#define SIZEEDIT_H

#include <QLineEdit>

class SizeEdit : public QLineEdit
{
	Q_OBJECT

public:
	SizeEdit(QWidget *parent = nullptr);
	QSize value() const;
	void setValue(QSize value);

private slots:
	void on_editingFinished();

signals:
	void sizeChanged(QSize size);

private:
	QSize size_;
};

#endif // SIZEEDIT_H
