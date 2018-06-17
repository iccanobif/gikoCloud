var fs = require('fs');

let defaultConfigValues = {
    "cli_path": "/src/src/cli/cli"
};

if (!fs.existsSync("config"))
    var configValues = {};
else
    var configValues = JSON.parse(fs.readFileSync("config", "utf8"));

module.exports.getParameter = (paramName) =>
{
    if (paramName in configValues)
        return configValues[paramName];
    else
        return defaultConfigValues[paramName];
};
