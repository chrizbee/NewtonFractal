// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

// Get canvas and setup gl context
var canvas = document.getElementById("glcanvas");
canvas.width = window.innerWidth;
canvas.height = window.innerHeight;
var gl = canvas.getContext("webgl");
if (!gl) gl = canvas.getContext("experimental-webgl");
if (!gl) alert("Your browser does not support WebGL");
var program;
const vertices = [
	-1.0, -1.0, 0.0,
	 1.0, -1.0, 0.0,
	 1.0,  1.0, 0.0,
	-1.0,  1.0, 0.0
];

// Setup position and dragger
const DraggingMode = { NoDragging: 'no', DraggingRoot: 'root', DraggingFractal: 'fractal' };
var dragger = {
	mode: DraggingMode.NoDragging,
	previousPos: new Point(0, 0),
	index: -1
}

// Setup parameters
var params = new Parameters();
for (i = 0; i < nf.DRC; ++i) {
	params.roots.push(new Root(new Complex(0, 0), nf.predefColors[i]));
}
params.reset();

// Bind resize event to function
window.addEventListener("resize", resizeGL);
canvas.onmousedown = mousePressEvent;
canvas.onmouseup = mouseReleaseEvent;
canvas.onmousemove = mouseMoveEvent;
canvas.onwheel = mouseWheelEvent;

// Manually initialize WebGL and reset params
initGL();
resizeGL();
paintGL();

function initGL() {
	// Disable depth test since it's only 2D
	gl.disable(gl.DEPTH_TEST);

	// Load shader
	var vshaderText = document.getElementById("vertex-shader").text;
	var fshaderText = document.getElementById("fragment-shader").text;
	var vshader = gl.createShader(gl.VERTEX_SHADER);
	var fshader = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(vshader, vshaderText);
	gl.shaderSource(fshader, fshaderText);
	gl.compileShader(vshader);
	if (!gl.getShaderParameter(vshader, gl.COMPILE_STATUS)) {
		console.error("Error compiling the vertex shader!", gl.getShaderInfoLog(vshader));
		return;
	}
	gl.compileShader(fshader);
	if (!gl.getShaderParameter(fshader, gl.COMPILE_STATUS)) {
		console.error("Error compiling the fragment shader!", gl.getShaderInfoLog(fshader));
		return;
	}

	// Create, link, validate and bind program
	program = gl.createProgram();
	gl.attachShader(program, vshader);
	gl.attachShader(program, fshader);
	gl.linkProgram(program);
	if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
		console.error("Error linking the program!", gl.getProgramInfoLog(program));
		return;
	}
	gl.validateProgram(program);
	if (!gl.getProgramParameter(program, gl.VALIDATE_STATUS)) {
		console.error("Error validating the program!", gl.getProgramInfoLog(program));
		return;
	}
	gl.useProgram(program);

	// Set attribute array and static uniforms
	var verticesBufferObject = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, verticesBufferObject);
	gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
	gl.uniform1f(gl.getUniformLocation(program, "EPS"), nf.EPS);
	gl.uniform1f(gl.getUniformLocation(program, "RIR"), params.distance2complex(nf.RIR));
}

function paintGL() {
	// Bind program and enable vertices
	gl.useProgram(program);
	gl.enableVertexAttribArray(0);
	gl.vertexAttribPointer(0, 3, gl.FLOAT, gl.FALSE, 3 * Float32Array.BYTES_PER_ELEMENT, 0);

	// Paint with new params
	gl.uniform1i(gl.getUniformLocation(program, "rootCount"), params.roots.length);
	gl.uniform4fv(gl.getUniformLocation(program, "limits"), params.limits.vec4());
	gl.uniform1i(gl.getUniformLocation(program, "maxIterations"), params.maxIterations);
	gl.uniform2f(gl.getUniformLocation(program, "damping"), params.damping.real, params.damping.imag);
	gl.uniform2f(gl.getUniformLocation(program, "size"), canvas.width, canvas.height);
	gl.uniform2fv(gl.getUniformLocation(program, "roots"), params.rootsVec2());
	gl.uniform3fv(gl.getUniformLocation(program, "colors"), params.colorsVec3());
	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
}

function resizeGL() {
	// Resize canvas and limits to window size
	params.limits.resize(window.innerWidth - canvas.width, window.innerHeight - canvas.height);
	canvas.width = window.innerWidth;
	canvas.height = window.innerHeight;
	gl.viewport(0, 0, canvas.width, canvas.height);
	paintGL();
}

function mousePressEvent(event) {
	// Set cursor and dragger
	dragger.previousPos.set(event.clientX, event.clientY);
	let i = params.rootContainsPoint(new Point(event.clientX, event.clientY));

	// Cursor pressed on root
	if (i >= 0) {
		dragger.mode = DraggingMode.DraggingRoot;
		dragger.index = i;
	
	// Cursor pressed on fractal
	} else {
		dragger.mode = DraggingMode.DraggingFractal;
	}
}

function mouseMoveEvent(event) {
	// Move root if dragging
	var pos = new Point(event.clientX, event.clientY);
	if (dragger.mode == DraggingMode.DraggingRoot && dragger.index >= 0 && dragger.index < params.roots.length) {
		params.roots[dragger.index].value = params.point2complex(pos);
		canvas.style.cursor = "grabbing";
		paintGL();
		
	// Else move fractal
	} else if (dragger.mode == DraggingMode.DraggingFractal) {
		params.limits.move(dragger.previousPos.x - pos.x, dragger.previousPos.y - pos.y);
		dragger.previousPos.set(pos.x, pos.y);
		canvas.style.cursor = "move";
		paintGL();
		
	// Else if over root change cursor
	} else {
		if (params.rootContainsPoint(pos) >= 0) {
			console.log("Mouseover root");
			canvas.style.cursor = "grab";
		} else canvas.style.cursor = "default";
	}
}

function mouseReleaseEvent(event) {
	// Reset dragger and repaint
	dragger.mode = DraggingMode.NoDragging;
	dragger.index = -1;
	paintGL();
}

function mouseWheelEvent(event) {
	// Calculate weight
	let xw = event.clientX / canvas.width;
	let yw = event.clientY / canvas.height;

	// Zoom fractal in / out
	let zoomin = event.deltaY < 0;
	params.limits.zoom(zoomin, xw, yw);
	paintGL();
}