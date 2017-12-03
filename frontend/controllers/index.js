const allBlocks = require('../models/blocks.js');
// Create a function which is a "controller", it
// handles a request, writing the response.
/*function index(request, response) {
    response.render('index', { title: 'BlockChain Monitor' });
}*/
/*
function index0(req, res) {
  EventModel.getAllEvents((err, events) => {
    const context = {
      title: 'List of Events',
      events: events,
    };
    res.render('index', context);
  });
}

module.exports = {
    index,
};
*/

// Create a function which is a "controller", it
// handles a request, writing the response.
function listBlocks(request, response) {
  // Store the value of the `q` GET parameter in the
  // `query` variable.
    const query = request.query.q;
    const contextData = {
        title: 'List of blocks',
        blockMatchignQuery: allBlocks,
    };
    response.render('index', contextData);
}

module.exports = {
    listBlocks,
};
