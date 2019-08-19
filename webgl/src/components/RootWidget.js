import React, { Component } from 'react'
import Context from '../Context';
import ComplexInput from './ComplexInput';
import { Button, Position, Menu, MenuItem, Popover, PopoverInteractionKind } from '@blueprintjs/core';

class RootWidget extends Component {
    static contextType = Context;

    onRootChanged(complex) {
		// Update parameters and notify parent
        this.context.parameters.roots[this.context.parameters.roots.indexOf(this.props.value)].value = complex;
        this.props.onChange(false);
    }
    
    removeRoot(root) {       
        this.context.parameters.roots.splice(this.context.parameters.roots.indexOf(root),1);
        this.props.onChange(true);
	}

    render() {
        const { id, value, decimalCount, overWriteValue, leftIcon, isOpen } = this.props;
        const { parameters } = this.context;
        const rootMenu = (
            <Popover
                content={
                    <Menu>
                        {parameters.roots.length > 2 && <MenuItem text="Remove root" onClick={() => this.removeRoot(value)} />}
                        <MenuItem text="Change color" />
                        <MenuItem text="Mirror on x-axis" />
                        <MenuItem text="Mirror on y-axis" />
                    </Menu>
                }
                disabled={!isOpen}
                position={Position.LEFT_TOP}
                interactionKind={PopoverInteractionKind.CLICK}
            >
                <Button minimal={true} rightIcon="caret-down" />
            </Popover>
        );

        return (
            <ComplexInput
                key={id} overWriteValue={overWriteValue} value={value.value}
                onValueChange={(complex) => this.onRootChanged(complex)}
                decimalCount={decimalCount} leftIcon={leftIcon} fill={true} rightElement={rootMenu} />
        )
    }
}

export default RootWidget;