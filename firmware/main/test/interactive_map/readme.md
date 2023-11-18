# Install dependencies
- `brew install npm websocat`
- `cd test/interactive_map; npm install`
- `cd generated; npm install`

# Running Interactive Map Test
1. Compile for `host` target.
2. `npm start` - Starts the webserver with the map.
3. Go to http://localhost:5173
4. Run the host version, and pipe the websocket data into it - `websocat -s 9998 | ./.pio/build/host/program`
