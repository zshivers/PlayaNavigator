var fs = require("fs");
var turf = require('turf');

var content = fs.readFileSync("./Portable_Toilets.json");
var toilet_data = JSON.parse(content);

for (let t in toilet_data["features"]) {
    let feature = toilet_data["features"][t]
    let centroid = turf.centroid(feature)["geometry"]
    console.log("{" + centroid["coordinates"][1] + ", " + centroid["coordinates"][0] + "},")
}