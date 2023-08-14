import './style.css';
import { Map, View } from 'ol';
import TileLayer from 'ol/layer/Tile';
import Feature from 'ol/Feature.js';
import { Icon, Style } from 'ol/style.js';
import { Modify } from 'ol/interaction.js';
import Point from 'ol/geom/Point.js';
import OSM from 'ol/source/OSM';
import * as olProj from 'ol/proj';
import GeoJSON from 'ol/format/GeoJSON.js';
import VectorLayer from 'ol/layer/Vector';
import VectorSource from 'ol/source/Vector';

// Open websocket to send coordinates to.
var ws = new WebSocket("ws://localhost:9998/");

const view = new View({
  center: olProj.fromLonLat([-119.203500, 40.786400]),
  zoom: 14
});

// Icon used to indicate the current location.
const iconFeature = new Feature({
  geometry: new Point(olProj.fromLonLat([-119.203500, 40.786400])),
  name: 'Current Location Marker',
});

const vectorSource = new VectorSource({
  features: [iconFeature],
});

const vectorLayer = new VectorLayer({
  source: vectorSource,
});

// Create a bamp with the BM GeoJSON data.
const map = new Map({
  target: 'map',
  layers: [
    // new TileLayer({
    //   source: new OSM()
    // }),
    new VectorLayer({
      source: new VectorSource({
        format: new GeoJSON(),
        url: './data/Street_Outlines.json',
      }),
    }),
    new VectorLayer({
      source: new VectorSource({
        format: new GeoJSON(),
        url: './data/Portable_Toilets.json',
      }),
    }),
    new VectorLayer({
      source: new VectorSource({
        format: new GeoJSON(),
        url: './data/Promenades_Burns.json',
      }),
    }),
    vectorLayer
  ],
  view: view
});

// Allow the icon to be dragged to change the current location.
const target = document.getElementById('map');
const modify = new Modify({
  hitDetection: vectorLayer,
  source: vectorSource,
});
modify.on(['modifystart', 'modifyend'], function (evt) {
  target.style.cursor = evt.type === 'modifystart' ? 'grabbing' : 'pointer';
  if (evt.type === 'modifyend') {
    let centerLocation = olProj.toLonLat(iconFeature.getGeometry().getCoordinates());
    console.log(centerLocation);
    ws.send([centerLocation[1], centerLocation[0]]);
  }
});
const overlaySource = modify.getOverlay().getSource();
overlaySource.on(['addfeature', 'removefeature'], function (evt) {
  target.style.cursor = evt.type === 'addfeature' ? 'pointer' : '';
});

map.addInteraction(modify);