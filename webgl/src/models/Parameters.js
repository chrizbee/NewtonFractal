// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

import nf from "./Defaults";
import Limits from "./Limits";
import Complex from "complex.js"
import { Point, Root } from "./Root";

class Parameters {
	constructor() {
		this.limits = new Limits();
		this.maxIterations = nf.DMI;
		this.damping = new Complex(nf.DDP, 0.0);
		this.setRoots(nf.DRC);
	}

	setRoots(DRC) {
		this.roots = [];
		for (var i = 0; i < DRC; ++i) {
			this.roots.push(new Root(new Complex(0, 0), nf.predefColors[i]));
		}
		this.reset();
	}

	reset() {
		let rootCount = this.roots.length;
		for (var i = 0; i < rootCount; ++i) {
			var angle = 2 * Math.PI * i / rootCount;
			this.roots[i].value = new Complex(Math.cos(angle), Math.sin(angle));
		}
		this.limits.reset();
	}

	complex2point(z) {
		let x = (z.re - this.limits.left) * (window.innerWidth - 1) / this.limits.width();
		let y = (z.im - this.limits.top) * (window.innerHeight - 1) / -this.limits.height();
		return new Point(x, y);
	}

	point2complex(p) {
		let re = p.x * this.limits.width() / (window.innerWidth - 1) + this.limits.left;
		let im = p.y * -this.limits.height() / (window.innerHeight - 1) + this.limits.top;
		return new Complex(re, im);
	}

	distance2complex(d) {
		return (d * this.limits.width() / (window.innerWidth - 1));
	}

	rootContainsPoint(p) {
		let rootCount = this.roots.length;
		for (var i = 0; i < rootCount; ++i) {
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
		for (var i = 0; i < rootCount; ++i) {
			let value = this.roots[i].value.toVector();
			vec2.push(value[0], value[1]);
		}
		return vec2;
	}

	colorsVec3() {
		var vec3 = [];
		let rootCount = this.roots.length;
		for (var i = 0; i < rootCount; ++i) {
			let color = this.roots[i].color;
			vec3.push(color[0], color[1], color[2]);
		}
		return vec3;
	}
}

const parameters = new Parameters();
export default parameters;