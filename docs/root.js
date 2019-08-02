// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

class Point {
	constructor(x, y) {
		this.x = x;
		this.y = y;
	}

	set(x, y) {
		this.x = x;
		this.y = y;
	}

	distance(other) {
		return new Point(this.x - other.x, this.y - other.y);
	}
}

class Complex {
	constructor(real, imag) {
		this.real = real;
		this.imag = imag;
	}
}

class Root {
	constructor(value, color) {
		this.value = value;
		this.color = color;
	}

	valueVec2() {
		return [this.value.real, this.value.imag];
	}
}