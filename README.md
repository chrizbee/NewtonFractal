![Icon](resources/icons/Illustrator/icon64.png) ![Name](resources/icons/Illustrator/name64.png)

[![built with](https://img.shields.io/badge/built%20with-Qt-41cd52.svg)](https://www.qt.io/) [![contributors](https://img.shields.io/github/contributors/chrizbee/NewtonFractal.svg)](https://GitHub.com/chrizbee/NewtonFractal/graphs/contributors/) [![release](https://img.shields.io/github/release/chrizbee/NewtonFractal.svg)](https://GitHub.com/chrizbee/NewtonFractal/releases/)  [![license](https://img.shields.io/github/license/chrizbee/NewtonFractal.svg)](https://github.com/chrizbee/NewtonFractal/blob/master/LICENSE) [![stars](https://img.shields.io/github/stars/chrizbee/NewtonFractal.svg?color=ffdd00)](https://GitHub.com/chrizbee/NewtonFractal/stargazers/)

## Basic Overview
**NewtonFractal** is a *Qt5-Application* that renders fractals based on [*Newton's method*](https://en.wikipedia.org/wiki/Newton%27s_method). The application provides the following features:

![roots](resources/images/roots.gif) ![move](resources/images/move.gif) ![zoom](resources/images/zoom.gif) ![orbit](resources/images/orbit.gif) ![position](resources/images/position.gif) 

- Move up to 6 roots with drag & drop
- Move fractal
- Zoom in and out
- Orbit mode to visualize iterations
- Show the current cursor position as a complex number
- Set fractal size and downscaling factor (smooth rendering while moving)
- Change maximum number of newton iterations
- Change damping factor of newton's method
- Multi- or singlethreading
- Export fractal as png

## Getting Started

Get started by either downloading the current release (for *Windows*) or by building it with `qmake` and `make` (for *Windows*, *Linux* and *Mac*).

### Prerequisites

To build this application, you will need Qt5. If you are going to build on Windows, you need to make sure, that your `PATH` variable contains paths to *Qt* and *MinGW* / *MSVC* toolsets (*bin* folder).

### Building

Clone the GitHub repository
```bash
git clone https://github.com/chrizbee/NewtonFractal NewtonFractal
cd NewtonFractal
```

Build it using `qmake` and `make`
```bash
qmake
make [release | debug]
```
*Note*: The `release` build will be much faster than `debug`.

Run the application
```bash
cd build
./NewtonFractal
```

## Deployment

- **Linux** - [linuxdeployqt](https://github.com/probonopd/linuxdeployqt)
- **Windows** - [windeployqt](https://doc.qt.io/qt-5/windows-deployment.html)

## Built With

* [Qt5](https://www.qt.io/) - The UI framework used

## Versioning

We use [SemVer](http://semver.org/) for versioning. The current version is stored in [NewtonFractal.pro](NewtonFractal.pro) (`VERSION`).

## Authors

- [chrizbee](https://github.com/chrizbee)
- [opit7](https://github.com/opit7)

See also the list of [contributors](https://github.com/chrizbee/NewtonFractal/contributors) who participated in this project.

## License

This project is licensed under the GNU General Public License v3 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments & Sources

- Inspired by [ActiveState](http://code.activestate.com/recipes/577166-newton-fractals/)
- Icons by [Icons8](https://icons8.com/)
