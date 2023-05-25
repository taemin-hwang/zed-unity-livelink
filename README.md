# Stereolabs ZED - Live Link for Unity
ZED LiveLink Plugin for Unity

## ZED Live Link for Unity

This tool is an interface between the SDK Fusion module and Unity. It sends fused skeleton data to the engine so that 3D avatars can be animated in Unity using one or several ZED cameras working together.

The data is broadcast via UDP on a specified port, and can be received using the assets of the paired Unity project.

## Compatibility

### Dependencies

- **ZED SDK v4.x**, available on [stereolabs.com](https://www.stereolabs.com/developers/)
- **Unity 2021.3** and more recent versions, available on [unity.com](https://unity.com/download)
    - It will probably work well with older versions, as its core is an UDP receiver which animates an Humanoid avatar, but has not been tested with them.
    - You may encounter an error about the package **Newtonsoft** not being installed at first launch. With versions 2022.x and more recent of Unity, it should not happen, but in any case please refer to [the Newtonsoft repo](https://github.com/jilleJr/Newtonsoft.Json-for-Unity/wiki/Install-official-via-UPM) for installation instructions with Unity Package Manager.

### Difference with the ZED Live Link for Unity

All of this code is based on the ZED LIVE LINK FOR UNITY source code.

- [zed live link for unity](https://github.com/stereolabs/zed-unity-livelink)

The difference is just that you can control the behavior with a configuration file, and you can save and replay fusion data.

- [custom live link](custom-unity-livelink-fusion/README.md)