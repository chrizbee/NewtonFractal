// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

import React from "react";
import SettingsWidget from "./SettingsWidget"
import Canvas from "./Canvas"
import "./styles.css";

function App() {
	return (
		<div className="app">
			<SettingsWidget className="settingsWidget"/>
			<Canvas className="fractalWidget"/>
		</div>
	);
}

export default App;
