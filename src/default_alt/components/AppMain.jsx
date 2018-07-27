import React from 'react'
import SubComponent from 'SubComponent';
import Index from 'Index';
import darkBaseTheme from 'material-ui/styles/baseThemes/darkBaseTheme';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider'
import getMuiTheme from 'material-ui/styles/getMuiTheme'
import injectTapEventPlugin from 'react-tap-event-plugin';
import AppBar from 'material-ui/AppBar';
import RaiseButton from 'material-ui/RaisedButton'

class AppMain extends React.Component {

    constructor(props) {
        super(props);
    }

    componentWillMount() {
        // Inject Tap Event
        // Only in this component
        injectTapEventPlugin();
    }

    render() {
        return (
            <div>
                <MuiThemeProvider muiTheme={getMuiTheme(darkBaseTheme)} >
                    <div>
                        <AppBar title="Control" />
                        <SubComponent />
                    </div>
                </MuiThemeProvider>
            </div>
        )
    }
}

export default AppMain;
