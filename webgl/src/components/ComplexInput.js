// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

import React from "react";
import Complex from "complex.js"
import { InputGroup, Intent } from "@blueprintjs/core";

class ComplexInput extends React.Component {
	constructor(props) {
		super(props);
		this.state = {
			inputString: this.complex2string(this.props.value),
			valid: false
		}
	}

	componentDidMount() {
		// Init valid
		this.setState({valid: this.isValid(this.state.inputString)});
	}

	componentWillReceiveProps(props) {
		if(this.isValid(props.value) && this.props.overWriteValue){
			this.setState({inputString: this.complex2string(props.value)})
		}
	}

	complex2string(c) {
		// Convert complex to string with res decimals
		let reStr = c.re.toFixed(this.props.decimalCount);
		let imStr = Math.abs(c.im).toFixed(this.props.decimalCount);
		let str = reStr + (c.im < 0 ? " - " : " + ") + imStr + "i";
		return str;
	}

	// Validate complex number
	isValid(input) {
		// Validate input
		var c;
		let valid = true;
		try {
			c = new Complex(input);
			valid = !c.isNaN();
		} catch (e) {
			valid = false;
		}

		// Update state
		valid ? this.setState({ valid: true }) : this.setState({ valid: false });

		return valid;
	}

	inputChanged(event) {
		this.setState({overWriteValue: false});
		this.setState({ inputString: event.target.value });
		if (this.isValid(event.target.value)) {
			this.props.onValueChange(new Complex(event.target.value));
		}
	}

	render() {
		const { valid, inputString } = this.state;
		const { disabled, fill, large, leftIcon, placeholder, rightElement, round, type } = this.props;
		return (
			<InputGroup
				disabled={disabled} fill={fill}
				large={large} leftIcon={leftIcon} intent={valid ? Intent.NONE : Intent.DANGER}
				placeholder={placeholder} rightElement={rightElement}
				round={round} type={type} value={inputString}
				onChange={(event) => this.inputChanged(event)}>
			</InputGroup>
		);
	}
}

export default ComplexInput;