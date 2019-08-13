// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

import React from "react";
import fractalWidget from "./FractalWidget";

class Canvas extends React.Component {
	componentDidMount() {
		// Get canvas and WebGL context
		var canvas = document.getElementById("glcanvas");
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
		fractalWidget.initializeGL(canvas);

		// Bind events to corresponding functions
		window.addEventListener("resize", fractalWidget.resizeGL.bind(fractalWidget));
		canvas.onmousedown = fractalWidget.mousePressEvent.bind(fractalWidget);
		canvas.onmouseup = fractalWidget.mouseReleaseEvent.bind(fractalWidget);
		canvas.onmousemove = fractalWidget.mouseMoveEvent.bind(fractalWidget);
		canvas.onwheel = fractalWidget.mouseWheelEvent.bind(fractalWidget);

		// Resize and repaint
		fractalWidget.resizeGL();
	}

	render() {
		return (
			<canvas id="glcanvas"></canvas>
		);
	}
}

export default Canvas;