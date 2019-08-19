// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

import React from "react";
import Context from '../Context';
import ComplexInput from "./ComplexInput";
import Root from "./RootWidget";
import { slide as SettingsMenu } from "react-burger-menu";
import SimpleBar from 'simplebar-react';
import { NumericInput, FormGroup, Label, InputGroup } from "@blueprintjs/core";
import "../styles.css";
import 'simplebar/dist/simplebar.min.css';

class SettingsWidget extends React.Component {
	static contextType = Context;

	constructor(props) {
		super(props);
		this.state = {
			overWriteValue: false,
			isOpen: true,
			size: window.innerWidth.toString() + " x " + window.innerHeight.toString()
		}

		// Bind key event handler
		document.onkeyup = this.keyPressEvent.bind(this);
	}

	updateParametersAndUi() {
		this.context.updateParameters(this.context.parameters);
		this.context.updateUi();
	}

	// Called from App.js
	updateSettings() {
		this.setState({ overWriteValue: true });
		this.forceUpdate();
		this.setState({ overWriteValue: false });
	}

	onStateChanged(state) {
		// Update SettingsWidget state from Menu state
		this.setState({ isOpen: state.isOpen });
	}

	onZoomChanged(number) {
		// Update parameters and update
		this.context.parameters.limits.setZoomFactor(number / 100.0);
		this.updateParametersAndUi();
	}

	onIterationsChanged(number) {
		// Update parameters and update
		this.context.parameters.maxIterations = number;
		this.updateParametersAndUi();
	}

	onDegreeChanged(number) {
		// Update parameters and update
		this.context.parameters.roots = this.context.parameters.getRoots(number);
		this.updateParametersAndUi();
	}

	onDampingChanged(number) {
		// Update parameters and update
		this.context.parameters.damping = number;
		this.updateParametersAndUi();
	}

	keyPressEvent(event) {
		// Which key
		let ascii = event.keyCode;
		switch (ascii) {
			case 83: this.setState(state => ({ isOpen: !state.isOpen })); break;
			case 79: console.log("o was pressed"); break;
			case 80: console.log("p was pressed"); break;
			default: break;
		}
	}

	render() {
		const { isOpen, overWriteValue, size } = this.state;
		const { parameters } = this.context;
		return (
			<SettingsMenu className="settingsWidget"
				right width={320} noTransition noOverlay disableAutoFocus
				customBurgerIcon={false} customCrossIcon={false}
				isOpen={isOpen} onStateChange={(state) => this.onStateChanged(state)}>
				<div className="settingsBlock" >
					<FormGroup className="bp3-dark">
						<div className="title">
							<h2>NewtonFractal WebGL</h2>
						</div>
						<div className="settingsItems">
							<div className="item">
								<Label>Fractal size</Label>
								<InputGroup
									disabled value={size} leftIcon="fullscreen" fill={true}>
								</InputGroup>
							</div>
							<div className="item">
								<Label>Zoom factor (%)</Label>
								<NumericInput
									value={(parameters.limits.zoomFactor() * 100.0).toFixed(0)} onValueChange={(number) => this.onZoomChanged(number)}
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
									value={parameters.damping} onValueChange={(number) => this.onDampingChanged(number)}
									decimalCount={2} leftIcon="derive-column" fill={true}>
								</ComplexInput>
							</div>
						</div>
					</FormGroup>
					<hr className="divider" />
				</div>
				<div className="rootsBlock">
					<SimpleBar style={{ height: '100%' }}>
						<FormGroup className="bp3-dark">
							<div className="rootsItems">
								{parameters.roots.map((root, i) =>
									<div className="item">
										<Root
											id={i} value={root} decimalCount={2}
											overWriteValue={overWriteValue} isOpen={isOpen}
											onChange={(overWriteValue) => {
												this.updateParametersAndUi();
											}} />
									</div>
								)}
							</div>
						</FormGroup>
					</SimpleBar>
				</div>
				<div className="infoBlock" />
			</SettingsMenu>
		);
	}
}

export default SettingsWidget;