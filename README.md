# yapre

![yapre](./banner.png)

**Build Status**

|windows|mac|linux|emscripten|
|-------|-------|-------|-------|
|![Build Status](https://github.com/wo1fsea/yapre/actions/workflows/windows_build.yml/badge.svg)|![Build Status](https://github.com/wo1fsea/yapre/actions/workflows/mac_build.yml/badge.svg)|![Build Status](https://github.com/wo1fsea/yapre/actions/workflows/linux_build.yml/badge.svg)|![Build Status](https://github.com/wo1fsea/yapre/actions/workflows/emscripten_build.yml/badge.svg)

## Intro

Here comes Yet Another Portable Retro Engine(yapre)!

It is a toy project for everyone to make retro games on any platform(for now Windows, MacOS, Linux, emscripten, iOS and Android are supported) using C++ and lua.

Try the demo app in your webbrowser by typping [yapre.run](https://yapre.run) in your address bar!


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

