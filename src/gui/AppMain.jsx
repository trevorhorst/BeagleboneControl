import React from 'react'
// import RadioManager from 'RadioManager';
import customBaseTheme from 'themes/customBaseTheme';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider'
import getMuiTheme from 'material-ui/styles/getMuiTheme'
import injectTapEventPlugin from 'react-tap-event-plugin';

var customMuiTheme = getMuiTheme( customBaseTheme )
// var rcmodule = require('api/radioCommands');
// var rc = require('api/radioCommands').radioCommands;


class AppMain extends React.Component {

    constructor(props) {
        super(props);
    }

    componentWillMount() {
        injectTapEventPlugin();
    }

    render() {
        return (
            <div>
                <MuiThemeProvider muiTheme={customMuiTheme} >
                    <div>
                        Hello World!
                    </div>
                </MuiThemeProvider>
            </div>
        )
    }
}

export default AppMain;
