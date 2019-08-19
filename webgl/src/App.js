// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

import React from "react";
import Context from "./Context";
import SettingsWidget from "./components/SettingsWidget";
import FractalWidget from "./components/FractalWidget";
import Parameters from "./models/Parameters";
import "./styles.css";

class App extends React.Component {

	// Sync parameters
	updateParameters = parameters => {
		this.setState({parameters})
	}

	// Update FractalWidget
	updateUi = () => {
		this.refs.fractalWidget.paintGL();
	}

	// Update SettingsWidget
	updateSettings = () => {
		this.refs.settingsWidget.updateSettings();
	}

	state = {
		parameters: Parameters,
		updateParameters: this.updateParameters,
		updateUi: this.updateUi,
		updateSettings: this.updateSettings
	}

	render() {
		return (
			<div className="app">
				<Context.Provider value={this.state}>
					<SettingsWidget className="settingsWidget" ref="settingsWidget" />
					<FractalWidget className="fractalWidget" ref="fractalWidget" />
				</Context.Provider>
			</div>
		)
	}
}

export default App;
