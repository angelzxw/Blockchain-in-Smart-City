'use strict';

const express = require('express');

const app = express();

// Import our controllers from their files. Notice how we're
// giving the `require` built-in function the path a file
// locally instead of a dependency that was installed as
// specified in our `package.json` file, like "express".
const indexControllers = require('./controllers/index.js');

// Configure our "templating engine", which is
// Mozilla's "Nunjucks" in this case.
const nunjucks = require('nunjucks');

// Through this configuration, Nunjucks will "tell"
// our Express app that it is handling the templates,
// so that when we call the `render` function on a
// response object, it will rely on Nunjucks.
nunjucks.configure('views', {
    autoescape: true,
    express: app,
});
app.set('view engine', 'html');

app.use(express.static(__dirname + '/public'));
// Now, attach our "controllers" to our "routes".
app.get('/', indexControllers.listBlocks);
app.use('/about', require('./controllers/about.js'));

// Start up the application and listen on the specified
// port, or default to port 4000.
app.listen(process.env.PORT || 3000);
