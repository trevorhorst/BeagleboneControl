/*
* Features: 
* - First JS file that is run when app loads
* - Attaches the React App to the HTML DOM
* - Specifies URL -> Component Mapping
*/

import React from 'react';
import ReactDOM from 'react-dom';

// Components
import AppMain from 'AppMain';

ReactDOM.render(<AppMain /> , document.getElementById('reactApp'));
