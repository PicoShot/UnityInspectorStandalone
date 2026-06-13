# <h1 align="center">UnityInspectorStandalone</h1>

<div align="center">

A powerful, universal runtime inspector and debugging tool for Unity games with support for both **Mono** and **IL2CPP** runtimes.

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Unity](https://img.shields.io/badge/Unity-Mono%20%7C%20IL2CPP-green.svg)](https://unity.com/)
[![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)](https://github.com/)
[![Status](https://img.shields.io/badge/status-Beta%20%7C%20Active%20Development-orange.svg)](https://github.com/)

</div>

<p align="center">
  <img src="Assets/screenshot.png" alt="UnityInspector Screenshot" width="80%">
</p>

<p align="center">
  <em>Find more screenshots in the <a href="Assets/">Assets/</a> directory.</em>
</p>

---

## Overview

**UnityInspector** is a single-DLL runtime inspection tool that intercepts the game presentation loop to draw a Dear ImGui interface. It allows you to explore, inspect, and modify internal game structures, components, properties, and execute methods in real-time. 

Whether you are debugging your own Unity builds, researching game mechanics, or writing plugins, UnityInspector gives you complete live runtime access.

---

## Key Highlights

* **Single DLL Proxy** - Drop-in installation with zero external dependencies.
* **Dual Engine Support** - Full compatibility with both **Mono** and **IL2CPP** runtimes.
* **Live Modification** - Edit fields, read/write properties, and invoke functions on the fly.

---

## Features

### Scene Hierarchy Browser
* Comprehensive tree view of all active and inactive `GameObject`s in the active scenes.
* Inactive game objects are clearly distinguished with visual indicators.
* Fast search and filter controls to locate specific objects.

### Component Inspector
* Lists all script components, colliders, renderers, and standard Unity modules attached to the selected `GameObject`.
* Search filter to isolate specific component classes.
* Structured tabs dividing properties, fields, and general parameters.

### Field Editor
* Direct read and write access to both **Instance** and **Static** fields.
* Input validation and native controls for common types:
  * **Numeric**: `int`, `float`, `double`
  * **Vectors**: `Vector2`, `Vector3`, `Vector4`
  * **Rotations**: `Quaternion`
  * **Colors**: `Color` with a built-in interactive color picker
  * **Booleans**: `bool` toggle checkboxes
  * **Strings**: View values inline

### Property Editor
* View and edit active properties at runtime.
* Safely invokes getters and setters through reflection or IL2CPP metadata APIs.

### Method Invoker
* Invoke any class method (including private, static, and instance methods) dynamically.
* Interactive parameter input boxes with type validation.
* Displays returning objects or primitive values directly in the console/UI.

### Transform Editor
* Dedicated window for manipulating positioning.
* Switch between **Local** and **World** coordinate spaces.
* Edit position vectors, rotation angles, and scaling factors instantly.

---

## Requirements & Installation

### Requirements
* **OS**: Windows 10 / 11 (x64)
* **Graphics API**: **DirectX 11** or **DirectX 12**
* **Unity Runtimes**: Mono / IL2CPP

> Vulkan is not officially supported at this time to avoid external Vulkan SDK requirements.

### Setup Instructions
1. Download the latest precompiled `winhttp.dll` from the [Releases](https://github.com/Rebzzel/kiero) tab.
2. Locate the root directory of your target Unity game (the folder containing the game executable).
3. Copy `winhttp.dll` into the game directory.
4. Launch the game.
5. Press the <kbd>INSERT</kbd> key on your keyboard to toggle the UnityInspector interface.

---