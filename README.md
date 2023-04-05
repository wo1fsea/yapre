# yapre

![yapre](./vis/png/banner.png)

**Build Status**

|windows|mac|linux|emscripten|iOS|Anodroid|
|-------|-------|-------|-------|------|-------|
|![Build Status](https://github.com/wo1fsea/yapre/actions/workflows/windows_build.yml/badge.svg)|![Build Status](https://github.com/wo1fsea/yapre/actions/workflows/mac_build.yml/badge.svg)|![Build Status](https://github.com/wo1fsea/yapre/actions/workflows/linux_build.yml/badge.svg)|![Build Status](https://github.com/wo1fsea/yapre/actions/workflows/emscripten_build.yml/badge.svg)|![Build Status](https://github.com/wo1fsea/yapre/actions/workflows/ios_build.yml/badge.svg)|![Build Status](https://github.com/wo1fsea/yapre/actions/workflows/android_build.yml/badge.svg)|

## Intro

Here comes Yet Another Portable Retro Engine(yapre)!

It is a toy project for everyone to make retro games on any platform(for now Windows, MacOS, Linux, emscripten, iOS and Android are supported) using C++ and lua.

Try the demo app in your webbrowser by typping [yapre.run](https://yapre.run) in your address bar!


## Examples

<table>
<tr>
<td>
<img width="320px" src="./docs/example1.gif">
</td>
<td>
<img width="320px" src="./docs/example2.gif">
</td>
</tr>
<tr>
<td>
<img width="320px" src="./docs/example3.gif">
</td>
<td>
<img width="320px" src="./docs/example4.gif">
</td>
</tr>
</table>

## Build

### windows

```
> git clone --recurse-submodules -j8 git@github.com:wo1fsea/yapre.git
> cd yapre
> .\build_scripts\build_windows.bat
```

### mac

```
> git clone --recurse-submodules -j8 git@github.com:wo1fsea/yapre.git
> cd yapre
> sh ./build_scripts/build_mac.sh
```

### linux

```
> git clone --recurse-submodules -j8 git@github.com:wo1fsea/yapre.git
> cd yapre
> sh ./build_scripts/build_linux.sh
```

### generate ios xcode project on mac

#### for devices

```
> git clone --recurse-submodules -j8 git@github.com:wo1fsea/yapre.git
> cd yapre
> sh ./build_scripts/build_ios.sh
```

#### for simulators

```
> git clone --recurse-submodules -j8 git@github.com:wo1fsea/yapre.git
> cd yapre
> sh ./build_scripts/build_ios_simulator.sh
```

### generate android studio project on mac

```
> git clone --recurse-submodules -j8 git@github.com:wo1fsea/yapre.git
> cd yapre
> sh ./build_scripts/build_android.sh
```

### build emscripten version on mac/linux

#### build

```
> git clone --recurse-submodules -j8 git@github.com:wo1fsea/yapre.git
> cd yapre
> sh ./build_scripts/build_emscripten.sh
```

#### test run

```
> sh ./build_scripts/run_emscripten.sh
```

## API

### System

Init()

Deinit()

Update()

yapre.platform

yapre.Exit()

yapre.AddTimer(time_ms, callback_function)

### Audio

yapre.Beep(freq, duration_ms)

### Video

yapre.RenderSize(width, height)

yapre.SetClearColor(r, g, b, a)

yapre.DrawSprite(texture_path, x, y, z, width, height, rotate, r, g, b)

### Input

yapre.BindKeyboardInputCallback(key, callback_function)

yapre.UnbindKeyboardInputCallback(key)

yapre.BindMouseInputCallback(key, callback_function)

yapre.UnbindMouseInputCallback(key)

yapre.BindTouchInputCallback(key, callback_function)

yapre.UnbindTouchInputCallback(key)
