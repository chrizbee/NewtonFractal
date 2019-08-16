// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

import React from "react";
import ComplexInput from "./ComplexInput";
import { slide as Menu } from "react-burger-menu";
import SimpleBar from 'simplebar-react';
import update from 'immutability-helper';
import { NumericInput, FormGroup, Label, InputGroup, Button, Position, MenuItem, Popover } from "@blueprintjs/core";
import "../styles.css";
import 'simplebar/dist/simplebar.min.css';

class SettingsWidget extends React.Component {
	constructor(props) {
		super(props);
		this.state = {
			parameters: this.props.parameters,
			isOpen: true,
			size: window.innerWidth.toString() + " x " + window.innerHeight.toString()
		}

		// Bind key event handler
		document.onkeyup = this.keyPressEvent.bind(this);
	}

	componentWillReceiveProps(props) {
		if (this.state.parameters === props.parameters) {return;}
		console.log("[SettingsWidget] Reiceived props:");
		console.log(props);
		this.setState({ parameters: props.parameters }, () => {
			
		});
	}

	onStateChanged(state) {
		// Update SettingsWidget state from Menu state
		this.setState({ isOpen: state.isOpen });
	}

	onZoomChanged(number) {
		// Update parameters and repaint
		// TODO: use state
		this.setState(update(this.state, {
			parameters: {
				limits: { $set: this.props.parameters.limits.setZoomFactor(number / 100.0) }
			}
		}), () => {
			this.props.onUpdate(this.state.parameters);
		});
	}

	onIterationsChanged(number) {
		console.log(number);
		// Update parameters and repaint
		this.setState(update(this.state, {
			parameters: {
				maxIterations: { $set: number }
			}
		}), () => {
			this.props.onUpdate(this.state.parameters);
		});
	}

	onDegreeChanged(number) {
		// Update parameters and repaint
		this.setState(update(this.state, {
			parameters: {
				roots: { $set: this.state.parameters.getRoots(number) }
			}
		}), () => {
			this.state.parameters.reset();
			this.props.onUpdate(this.state.parameters);
		});
	}

	onDampingChanged(number) {
		// Update parameters and repaint
		this.setState(update(this.state, {
			parameters: {
				damping: { $set: number }
			}
		}), () => {
			this.props.onUpdate(this.state.parameters);
		});
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
		const rootMenu = (
			<Popover
				content={
					<Menu>
						<MenuItem text="Remove root" />
						<MenuItem text="Change color" />
						<MenuItem text="Mirror on x-axis" />
						<MenuItem text="Mirror on y-axis" />
					</Menu>
				}
				disabled={false}
				position={Position.BOTTOM_RIGHT}
			>
				<Button disabled={false} rightIcon="caret-down" />
			</Popover>
		);

		return (
			<Menu className="settingsWidget"
				right width={320} noTransition noOverlay disableAutoFocus
				customBurgerIcon={false} customCrossIcon={false}
				isOpen={this.state.isOpen} onStateChange={(state) => this.onStateChanged(state)}>
				<div className="settingsBlock" >
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
									value={(this.state.parameters.limits.zoomFactor() * 100.0).toFixed(0)} onValueChange={(number) => this.onZoomChanged(number)}
									min={0.01} buttonPosition="right" leftIcon="zoom-in" fill={true}>
								</NumericInput>
							</div>
							<div className="item">
								<Label>Max number of iterations</Label>
								<NumericInput
									value={this.state.parameters.maxIterations} onValueChange={(number) => this.onIterationsChanged(number)}
									min={1} buttonPosition="right" leftIcon="repeat" fill={true}>
								</NumericInput>
							</div>
							<div className="item">
								<Label>Number of roots</Label>
								<NumericInput
									value={this.state.parameters.roots.length} onValueChange={(number) => this.onDegreeChanged(number)}
									min={2} max={10} buttonPosition="right" leftIcon="graph" fill={true}>
								</NumericInput>
							</div>
							<div className="item">
								<Label>Damping factor</Label>
								<ComplexInput
									value={this.state.parameters.damping} onValueChange={(number) => this.onDampingChanged(number)}
									res={2} leftIcon="derive-column" fill={true}>
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
								{this.state.parameters.roots.map((root, i) =>
									<div className="item">
										<ComplexInput
											key={i}
											value={root.value} onValueChange={(number) => this.onDampingChanged(number)}
											res={2} leftIcon="selection" fill={true} rightElement={rootMenu} />
									</div>
								)}
							</div>
						</FormGroup>
					</SimpleBar>
				</div>
				<div className="infoBlock" ></div>
			</Menu>
		);
	}
}

export default SettingsWidget;