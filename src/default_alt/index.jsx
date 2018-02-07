/*
* Features: 
* - First JS file that is run when app loads
* - Attaches the React App to the HTML DOM
* - Specifies URL -> Component Mapping
*/

import React from 'react';
import ReactDOM from 'react-dom';

// React Router
// import { Router, Route, Link, browserHistory, IndexRoute } from 'react-router';

// Components
import AppMain from 'AppMain';

// Polyfill Browser Compatability
// require('es6-promise').polyfill();
// 
// // Setup Routing
// var routerConfig =
// <Router history={browserHistory}>
// 	{/* Main App Bundle */}
// 	<Route path="/" component={AppMain} />
// </Router>

ReactDOM.render(<AppMain /> , document.getElementById('reactApp'));
