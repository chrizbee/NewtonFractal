// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

import React from "react";
import { vshader, fshader } from "../shader";
import nf from "../models/Defaults";
import { Point } from "../models/Root";
import update from 'immutability-helper';

// Drag root or fractal
const DraggingMode = {
	NoDragging: "no",
	DraggingRoot: "root",
	DraggingFractal: "fractal"
};

// Dragger
var dragger = {
	mode: DraggingMode.NoDragging,
	previousPos: new Point(0, 0),
	index: -1
}

class FractalWidget extends React.Component {
	constructor(probs) {
		super(probs);
		this.state = {
			parameters: this.props.parameters
		}
	}

	componentWillReceiveProps(props) {
		if (this.state.parameters === props.parameters) {return;}
		console.log("[FractalWidget] Reiceived props:");
		console.log(props);
		this.setState({ parameters: props.parameters }, () => {
			this.paintGL();
		});
	}

	componentDidMount() {
		// Get canvas and WebGL context
		var canvas = document.getElementById("glcanvas");
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
		this.initializeGL(canvas);

		// Bind events to corresponding functions
		window.addEventListener("resize", this.resizeGL.bind(this));
		canvas.onmousedown = this.mousePressEvent.bind(this);
		canvas.onmouseup = this.mouseReleaseEvent.bind(this);
		canvas.onmousemove = this.mouseMoveEvent.bind(this);
		canvas.onwheel = this.mouseWheelEvent.bind(this);

		// Resize and repaint
		this.resizeGL();
	}

	initializeGL(canvas) {
		// Compile shader, create and link program
		this.canvas = canvas;
		this.gl = this.canvas.getContext("webgl");
		if (!this.gl) this.gl = this.canvas.getContext("experimental-webgl");
		if (!this.gl) alert("Your browser does not support WebGL");
		this.program = this.gl.createProgram();
		this.addShaderFromText(vshader, this.gl.VERTEX_SHADER);
		this.addShaderFromText(fshader, this.gl.FRAGMENT_SHADER);
		this.linkProgram();
		this.setupVertices();
	}

	paintGL() {
		// Bind program and enable vertices
		this.gl.useProgram(this.program);
		this.gl.enableVertexAttribArray(0);
		this.gl.vertexAttribPointer(0, 3, this.gl.FLOAT, this.gl.FALSE, 3 * Float32Array.BYTES_PER_ELEMENT, 0);

		// Paint with new params
		this.gl.uniform1i(this.gl.getUniformLocation(this.program, "rootCount"), this.state.parameters.roots.length);
		this.gl.uniform4fv(this.gl.getUniformLocation(this.program, "limits"), this.state.parameters.limits.vec4());
		this.gl.uniform1i(this.gl.getUniformLocation(this.program, "maxIterations"), this.state.parameters.maxIterations);
		this.gl.uniform2f(this.gl.getUniformLocation(this.program, "damping"), this.state.parameters.damping.re, this.state.parameters.damping.im);
		this.gl.uniform2f(this.gl.getUniformLocation(this.program, "size"), this.canvas.width, this.canvas.height);
		this.gl.uniform2fv(this.gl.getUniformLocation(this.program, "roots"), this.state.parameters.rootsVec2());
		this.gl.uniform3fv(this.gl.getUniformLocation(this.program, "colors"), this.state.parameters.colorsVec3());
		this.gl.drawArrays(this.gl.TRIANGLE_FAN, 0, 4);
	}

	resizeGL() {
		// Resize canvas and limits to window size
		this.state.parameters.limits.resize(window.innerWidth - this.canvas.width, window.innerHeight - this.canvas.height);
		this.canvas.width = window.innerWidth;
		this.canvas.height = window.innerHeight;
		this.gl.viewport(0, 0, this.canvas.width, this.canvas.height);
		this.paintGL();
	}

	mousePressEvent(event) {
		// Set cursor and dragger
		dragger.previousPos.set(event.clientX, event.clientY);
		let i = this.state.parameters.rootContainsPoint(new Point(event.clientX, event.clientY));

		// Cursor pressed on root
		if (i >= 0) {
			dragger.mode = DraggingMode.DraggingRoot;
			dragger.index = i;

			// Cursor pressed on fractal
		} else {
			dragger.mode = DraggingMode.DraggingFractal;
		}
	}

	mouseMoveEvent(event) {
		// Move root if dragging
		var pos = new Point(event.clientX, event.clientY);
		if (dragger.mode === DraggingMode.DraggingRoot && dragger.index >= 0 && dragger.index < this.state.parameters.roots.length) {
			this.setState(update(this.state, {
				parameters: {
					roots: {
						[dragger.index]: {
							value: {$set: this.state.parameters.point2complex(pos)}
						}
					}
				}
			}));
			this.canvas.style.cursor = "grabbing";
			this.paintGL();

			// Else move fractal
		} else if (dragger.mode === DraggingMode.DraggingFractal) {
			this.state.parameters.limits.move(dragger.previousPos.x - pos.x, dragger.previousPos.y - pos.y);
			dragger.previousPos.set(pos.x, pos.y);
			this.canvas.style.cursor = "move";
			this.paintGL();

			// Else if over root change cursor
		} else {
			if (this.state.parameters.rootContainsPoint(pos) >= 0) {
				this.canvas.style.cursor = "grab";
			} else this.canvas.style.cursor = "default";
		}
	}

	mouseReleaseEvent(event) {
		// Reset dragger and repaint
		dragger.mode = DraggingMode.NoDragging;
		dragger.index = -1;
		this.paintGL();
	}

	mouseWheelEvent(event) {
		// Calculate weight
		let xw = event.clientX / this.canvas.width;
		let yw = event.clientY / this.canvas.height;

		// Zoom fractal in / out
		let zoomin = event.deltaY < 0;
		// TODO: use state
		this.state.parameters.limits.zoom(zoomin, xw, yw);
		this.props.onUpdate(this.state.parameters);
		this.paintGL();
	}

	addShaderFromText(text, type) {
		// Compile shader
		var shader = this.gl.createShader(type);
		this.gl.shaderSource(shader, text);
		this.gl.compileShader(shader);
		if (!this.gl.getShaderParameter(shader, this.gl.COMPILE_STATUS)) {
			console.error("Error compiling shader!", this.gl.getShaderInfoLog(shader));
			return;
		}

		// Attach shader to program
		this.gl.attachShader(this.program, shader);
	}

	linkProgram() {
		// Link program
		this.gl.linkProgram(this.program);
		if (!this.gl.getProgramParameter(this.program, this.gl.LINK_STATUS)) {
			console.error("Error linking the program!", this.gl.getProgramInfoLog(this.program));
			return;
		}

		// Validate program
		this.gl.validateProgram(this.program);
		if (!this.gl.getProgramParameter(this.program, this.gl.VALIDATE_STATUS)) {
			console.error("Error validating the program!", this.gl.getProgramInfoLog(this.program));
			return;
		}
	}

	setupVertices() {
		// Setup default vbo that will never change
		const vertices = [-1.0, -1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0, -1.0, 1.0, 0.0];
		var verticesBufferObject = this.gl.createBuffer();
		this.gl.useProgram(this.program);
		this.gl.bindBuffer(this.gl.ARRAY_BUFFER, verticesBufferObject);
		this.gl.bufferData(this.gl.ARRAY_BUFFER, new Float32Array(vertices), this.gl.STATIC_DRAW);
		this.gl.uniform1f(this.gl.getUniformLocation(this.program, "EPS"), nf.EPS);
		this.gl.uniform1f(this.gl.getUniformLocation(this.program, "RIR"), this.state.parameters.distance2complex(nf.RIR));
	}

	render() {
		return (
			<canvas id="glcanvas"></canvas>
		);
	}
}

export default FractalWidget;