// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

class Limits {
	constructor(original = false) {
		this.top = 1.0;
		this.right = 1.0;
		this.bottom = -1.0;
		this.left = -1.0;
		if (!original) {
			this.original = new Limits(true);
		} else this.original = null;
	}

	width() { 
		return this.right - this.left; 
	}

	height() { 
		return this.top - this.bottom; 
	}

	vec4() {
		return [this.top, this.right, this.bottom, this.left];
	}

	zoomFactor() {
		return this.original.width() / this.width();
	}

	setZoomFactor(zf) {
		let w2 = 0.5 * this.original.width() / zf;
		let h2 = 0.5 * this.original.height() / zf;
		let xMid = 0.5 * this.right + 0.5 * this.left;
		let yMid = 0.5 * this.top + 0.5 * this.bottom;
		this.top = yMid + h2;
		this.right = xMid + w2;
		this.bottom = yMid - h2;
		this.left = xMid - w2;
	}
	
	zoom(zoomin, xw, yw) {
		let zoom = zoomin ? -nf.ZMF : nf.ZMF;
		let wZoom = this.width() * zoom;
		let hZoom = this.height() * zoom;
		this.top += yw * hZoom;
		this.right += (1.0 - xw) * wZoom;
		this.bottom -= (1.0 - yw) * hZoom;
		this.left -= xw * wZoom;
	}

	move(x, y) {
		let dx = x * this.width() / (canvas.width - 1);
		let dy = y * -this.height() / (canvas.height - 1);
		this.top += dy;
		this.right += dx;
		this.bottom += dy;
		this.left += dx;
		if (this.original != null) {
			this.original.move(x, y);
		}
	}

	resize(x, y) {
		this.top += nf.DSF * y;
		this.right += nf.DSF * x;
		this.bottom -= nf.DSF * y;
		this.left -= nf.DSF * x;
		if (this.original != null) {
			this.original.resize(x, y);
		}
	}

	reset() {
		this.top = nf.DSF * canvas.height;
		this.right = nf.DSF * canvas.width;
		this.bottom = -nf.DSF * canvas.height;
		this.left = -nf.DSF * canvas.width;
		if (this.original != null) {
			this.original.reset();
		}
	}
}