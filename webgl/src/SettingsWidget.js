// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

import React from "react";
import parameters from "./Parameters";
import ComplexInput from "./ComplexInput";
import fractalWidget from "./FractalWidget";
import { slide as Menu } from "react-burger-menu";
import { NumericInput, FormGroup, Label, InputGroup } from "@blueprintjs/core";
import "./styles.css";

class SettingsWidget extends React.Component {
	constructor(props) {
		super(props);
		this.state = {
			isOpen: true,
			size: window.innerWidth.toString() + " x " + window.innerHeight.toString()
		}

		// Bind key event handler
		document.onkeyup = this.keyPressEvent.bind(this);
	}

	onStateChanged(state) {
		// Update SettingsWidget state from Menu state
		this.setState({isOpen: state.isOpen});
	}

	onZoomChanged(number) {
		// Update parameters and repaint
		parameters.limits.setZoomFactor(number / 100.0);
		fractalWidget.paintGL();
	}

	onIterationsChanged(number) {
		// Update parameters and repaint
		parameters.maxIterations = number;
		fractalWidget.paintGL();
	}

	onDegreeChanged(number) {
		// Update parameters and repaint
		parameters.setRoots(number);
		fractalWidget.paintGL();
	}

	onDampingChanged(number) {
		// Update parameters and repaint
		parameters.damping = number;
		fractalWidget.paintGL();
	}

	keyPressEvent(event) {
		// Which key
		let ascii = event.keyCode;
		switch (ascii) {
			case 83: this.setState(state => ({isOpen: !state.isOpen})); break;
			case 79: console.log("o was pressed"); break;
			case 80: console.log("p was pressed"); break;
			default: break;
		}
	}

	render() {
		return (
			<Menu className="settingsWidget"
				right width={320} noTransition noOverlay disableAutoFocus
				customBurgerIcon={false} customCrossIcon={false}
				isOpen={this.state.isOpen} onStateChange={(state) => this.onStateChanged(state)}>
				<FormGroup className="bp3-dark">
					<div className="title">
						<h2>NewtonFractal WebGL</h2>
					</div>
					<div className="settingsItems">
						<div className="item">
							<Label>Fractal size</Label>
							<InputGroup
								disabled value={this.state.size} leftIcon="fullscreen" fill={true}>
							</InputGroup>
						</div>
						<div className="item">
							<Label>Zoom factor (%)</Label>
							<NumericInput
								value={parameters.limits.zoomFactor() * 100.0} onValueChange={(number) => this.onZoomChanged(number)} 
								min={0.01} buttonPosition="right" leftIcon="zoom-in" fill={true}>
							</NumericInput>
						</div>
						<div className="item">
							<Label>Max number of iterations</Label>
							<NumericInput 
								value={parameters.maxIterations} onValueChange={(number) => this.onIterationsChanged(number)} 
								min={1} buttonPosition="right" leftIcon="repeat" fill={true}>
							</NumericInput>
						</div>
						<div className="item">
							<Label>Number of roots</Label>
							<NumericInput 
								value={parameters.roots.length} onValueChange={(number) => this.onDegreeChanged(number)}
								min={2} max={10} buttonPosition="right" leftIcon="graph" fill={true}>
							</NumericInput>
						</div>
						<div className="item">
							<Label>Damping factor</Label>
							<ComplexInput
								value={parameters.damping} leftIcon="derive-column" fill={true} onValueChange={(number) => this.onDampingChanged(number)}>
							</ComplexInput>
						</div>
					</div>
				</FormGroup>
			</Menu>
		);
	}
}

export default SettingsWidget;