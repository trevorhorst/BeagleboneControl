/*
    Use this file to configure the base color theme for the application.
*/

// For more info, check out the default material-ui theme at
// https://github.com/callemall/material-ui/blob/master/src/styles/baseThemes/lightBaseTheme.js
// https://github.com/callemall/material-ui/blob/master/src/styles/baseThemes/darkBaseTheme.js

import {
    lightBlueA400, lightBlueA200, lightBlue500, lightBlue700, lightBlue100,
    grey100, grey300, grey400, grey500, grey900,
    pinkA200, white, fullBlack, black
} from 'material-ui/styles/colors';
import {fade} from 'material-ui/utils/colorManipulator';

var secondaryText = '#727272'
var dividerColor = '#B6B6B6'

const customBaseTheme = {
	channels: ['blue', 'red', 'green', 'purple'],
    spacing: {
        iconSize: 24,
        desktopGutter: 24,
        desktopGutterMore: 32,
        desktopGutterLess: 16,
        desktopGutterMini: 8,
        desktopKeylineIncrement: 64,
        desktopDropDownMenuItemHeight: 32,
        desktopDropDownMenuFontSize: 15,
        desktopDrawerMenuItemHeight: 48,
        desktopSubheaderHeight: 48,
        desktopToolbarHeight: 56,
    },
    fontFamily: 'Roboto, sans-serif',
    palette: {
        primary1Color: '#003366',
        primary2Color: lightBlue500,
        primary3Color: grey400,
        accent1Color: '#cc7000',
        accent2Color: grey100,
        accent3Color: grey500,
        textColor: fullBlack,
        secondaryText: secondaryText,
        alternateTextColor: white,
        canvasColor: white,
        borderColor: grey300,
        disabledColor: fade(grey900, 0.3),
        pickerHeaderColor: lightBlue500,
        clockCircleColor: fade(grey900, 0.07),
        shadowColor: fullBlack,
    },
    // Component-Level Overrides
    appBar: {
        // by default, appbar uses primary1Color, but we want it to use white. So, we override
        // This only makes sense if we want ALL appbars used to be white.  Otherwise, just set backgroundColor white in styles
        color: 'white',
        textColor: lightBlue500
    }
};

export default customBaseTheme;