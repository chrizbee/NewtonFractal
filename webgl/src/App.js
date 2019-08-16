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
		parameters: parameters
	}

	// Sync parameters
	handleOnUpdate = (parameters) => {
		this.setState({parameters})
	}

	render() {
		return (
			<div className="app">
				<SettingsWidget className="settingsWidget" ref="settingsWidget"
				parameters={this.state.parameters} onUpdate={this.handleOnUpdate} />
				<FractalWidget className="fractalWidget" ref="fractalWidget"
				parameters={this.state.parameters} onUpdate={this.handleOnUpdate} />
			</div>
		)
	}
}

export default App;
