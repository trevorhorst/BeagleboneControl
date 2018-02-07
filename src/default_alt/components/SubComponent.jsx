import React from 'react'
import Dropzone from 'react-dropzone'
import FlatButton from 'material-ui/FlatButton';

import customBaseTheme from '../customBaseTheme';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider'
import getMuiTheme from 'material-ui/styles/getMuiTheme'
var customMuiTheme = getMuiTheme( customBaseTheme );

import API from 'api';

class SubComponent extends React.Component {

    constructor(props) {
        super(props);
    }

    onDrop(files) {

        // Actually send the file
        let comp = this;
        API.sendFile(files)
        .then(function(resp) {
            console.log(resp)
        })
        .catch(function(error) {
            console.log("ERROR", error)
        })
    }


    onUploadProgress( files ) {
        console.log( "Prgress.." )
    }

    render() {

        let dropzoneRef;

        return (
            <MuiThemeProvider muiTheme={customMuiTheme} >
                <div>
                    <Dropzone 
                        onUploadProgress={this.onUploadProgress.bind(this)}
                        onDrop={this.onDrop.bind(this)} 
                        ref={(node) => { dropzoneRef = node; }}  
                        disableClick
                    >
                        <p>Drop your config file here.</p>
                    </Dropzone>
                    <FlatButton label="Choose File" onClick={() => { dropzoneRef.open() }}/>
                </div>
            </MuiThemeProvider>
        )
    }
}

export default SubComponent;
