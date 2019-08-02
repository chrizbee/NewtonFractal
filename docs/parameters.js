// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

class Parameters {
	constructor() {
		this.roots = [];
		this.limits = new Limits();
		this.maxIterations = nf.DMI;
		this.damping = new Complex(nf.DDP, 0.0);
	}

	reset() {
		let rootCount = this.roots.length;
		for (i = 0; i < rootCount; ++i) {
			var angle = 2 * Math.PI * i / rootCount;
			this.roots[i].value = new Complex(Math.cos(angle), Math.sin(angle));
		}
		this.limits.reset();
	}

	complex2point(z) {
		let x = (z.real - this.limits.left) * (canvas.width - 1) / this.limits.width();
		let y = (z.imag - this.limits.top) * (canvas.height - 1) / -this.limits.height();
		return new Point(x, y);
	}

	point2complex(p) {
		let real = p.x * this.limits.width() / (canvas.width - 1) + this.limits.left;
		let imag = p.y * -this.limits.height() / (canvas.height - 1) + this.limits.top;
		return new Complex(real, imag);
	}

	distance2complex(d) {
		return (d * this.limits.width() / (canvas.width - 1));
	}

	rootContainsPoint(p) {
		let rootCount = this.roots.length;
		for (i = 0; i < rootCount; ++i) {
			var d = this.complex2point(this.roots[i].value).distance(p);
			if (Math.abs(d.x) < nf.RIR && Math.abs(d.y) < nf.RIR) {
				return i;
			}
		}
		return -1;
	}

	rootsVec2() {
		var vec2 = [];
		let rootCount = this.roots.length;
		for (i = 0; i < rootCount; ++i) {
			let value = this.roots[i].valueVec2();
			vec2.push(value[0], value[1]);
		}
		return vec2;
	}

	colorsVec3() {
		var vec3 = [];
		let rootCount = this.roots.length;
		for (i = 0; i < rootCount; ++i) {
			let color = this.roots[i].color;
			vec3.push(color[0], color[1], color[2]);
		}
		return vec3;
	}
}