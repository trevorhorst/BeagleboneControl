import React from 'react'
import RaisedButton from 'material-ui/RaisedButton';

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

    onHello( event ) {
        console.log( "hello" );
    }


    onUploadProgress( files ) {
        console.log( "Prgress.." )
    }

    render() {

        let dropzoneRef;

        return (
            <div>
                <RaisedButton label="Click Me!" onClick={this.onHello}/>
            </div>
        )
    }
}

export default SubComponent;
