// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

import React from "react";
import SettingsWidget from "./components/SettingsWidget";
import FractalWidget from "./components/FractalWidget";
import parameters from "./models/Parameters";
import "./styles.css";

class App extends React.Component {
	state = {
		test: "hello"
	}

	// Repaint canvas
	updateFractalWidget = () => {
		this.refs.fractalWidget.paintGL();
	}

	// Re-render SettingsWidget
	updateSettingsWidget = () => {
		this.refs.settingsWidget.forceUpdate();
	}

	render() {
		return (
			<div className="app">
				<SettingsWidget className="settingsWidget" ref="settingsWidget"
				parameters={parameters} onUpdate={this.updateFractalWidget} test={this.state.test} />
				<FractalWidget className="fractalWidget" ref="fractalWidget"
				parameters={parameters} onUpdate={this.updateSettingsWidget} test={this.state.test} />
			</div>
		)
	}
}

export default App;
