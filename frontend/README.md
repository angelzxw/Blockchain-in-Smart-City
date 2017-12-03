## Starter code for simple MVC Node.js homework

This is a minimal [Node.js](http://nodejs.org/)
application that shows very basic "model view controller"
structure and form handling.

What are all these files, you ask? Below you'll find a description of each one - hopefully this will help you get started.

* `server.js` - this is your server code. The `express` module is how you tell your application what to do with different requests -- check out the [Express website](http://expressjs.com) for more.
* `controllers/` - this directory contains our "controllers" or "handlers", functions that take an HTTP
request and write to the HTTP response. It is common practice to separate your controllers into different
files based on what part of the application they're handling. In this case, we've separated out the
controllers for the index page and the attendees page.
* `models/` - it is common practice to put your data storage into "models"---pieces of
code that "model" the structure of the data upon which your application depends. In this case, we
have just one model file and it stores only a list of names. Usually, our models would be "wired up"
to a database, but that's not the case here---we're just storing our data in javascript.
* `views/` - these are the "templates" or "views" for our application. These are usually HTML,
but with another "templating language" thrown in so that we can generate different HTML given
different data, like a mail merge. (There are many templating languages; we're using "nunjucks".)
The controller typically gathers the data
needed for an HTTP response, and renders the view using these data. (The data are often called
"context data".)
* `package.json` - this is Node's configuration file for your project. Inside you'll find information about the project, along with a list of dependencies (like Express) that you can install into `node_modules/` with `npm install`.
* `node_modules/` - the folder containing all of your dependencies. Usually you don't need to do anything to it, but if your app is misbehaving, sometimes removing and reinstalling `node_modules/` by running `rm -rf node_modules/` and `npm install` or `yarn install` will fix it.
* `yarn.lock` - this describes the exact state of your `node_modules/` tree without uploading the tree itself. npm modules are always being updated, so this allows you to know exactly what you're running and standardize this across installations.
* `.gitignore` - this tells Git which files and folders to ignore -- for example, you don't want to push `node_modules/` to GitHub.
* `Procfile` - this is how you tell Heroku how to run your code. You shouldn't need to mess with this one.
* `.eslintrc.json` - this is where we customize the rules for how your code gets linted (checked for style/errors) - most of them come from the `eslint-config-airbnb` package. You also shouldn't need to modify this one (please don't, actually). If you're working on cloud9, their built-in editor will automatically use your eslint configuration. If you're working in another editor, it will depend on your setup.
* `README.md` - this file :)

## Installing dependencies

To install dependencies, run `yarn install`. Of course, you'll want yarn installed.
You can install that with `npm install yarn` optionally adding the `-g` flag if you
want to install it "globally", which will depend on your preferences.


## Running the code

To run this application in a development environment, use the command `yarn dev`.
That will run the "dev" script defined in `package.json`, which itself runs
[nodemon](https://github.com/remy/nodemon). Nodemon will automatically restart
your application when it detects changes to your code. Each time it restarts your
application it will run [ESLint](https://eslint.org) on your code first to detect
common problems and enforce the JavaScript style specified in your `.eslintrc.json`
file.

In production, you should start the app with the command either `yarn start` or
`npm start`. On Heroku, this will happen by default, in the absense of a Procfile.
