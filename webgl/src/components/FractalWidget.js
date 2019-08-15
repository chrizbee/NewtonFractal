// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

import React from "react";
import fractal from "../models/Fractal";

class FractalWidget extends React.Component {
	componentDidMount() {
		// Get canvas and WebGL context
		var canvas = document.getElementById("glcanvas");
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
		fractal.initializeGL(canvas);

		// Bind events to corresponding functions
		window.addEventListener("resize", fractal.resizeGL.bind(fractal));
		canvas.onmousedown = fractal.mousePressEvent.bind(fractal);
		canvas.onmouseup = fractal.mouseReleaseEvent.bind(fractal);
		canvas.onmousemove = fractal.mouseMoveEvent.bind(fractal);
		canvas.onwheel = fractal.mouseWheelEvent.bind(fractal);

		// Resize and repaint
		fractal.resizeGL();
	}

	render() {
		return (
			<canvas id="glcanvas"></canvas>
		);
	}
}

export default FractalWidget;