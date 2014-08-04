/**
 * GET homepage
 */
exports.index = function (req, res) {
  res.render('index', {
    title : 'Project'
  })
}

exports.page = function (req, res) {
  res.render('page', {
    title : 'Project'
  })
}
