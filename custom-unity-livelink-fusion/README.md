# Custom Live Link for Unity


## How to build
```
./run_build
```

### How to configure
```
cd etc
cat config.json
```
- **addr**: ip address for Unity server
- **port**: port number for Unity server
- **viewer** : enable/disable OpenGL viewer
- **resolution** : ZED camera resolution
- **fps** : target fps
- **accuracy** : fast(0), medium(1), accurate(2)

### How to run
```
cd build
./Custom-LiveLink-Fusion ../etc/calibration.json # Need to use your own calibration file
```
