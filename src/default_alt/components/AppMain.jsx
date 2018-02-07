import React from 'react'
import SubComponent from 'SubComponent';

class AppMain extends React.Component {

    constructor(props) {
        super(props);
    }

    componentWillMount() {
        // Inject Tap Event
        // Only in this component
    }

    render() {
        return (
            <div>
                HelloWorld!
            </div>
        )
    }
}

export default AppMain;
