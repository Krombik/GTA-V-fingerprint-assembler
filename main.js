// Modules to control application life and create native browser window
const { app, BrowserWindow, globalShortcut } = require('electron');
const fs = require('fs');
const Buffer = require("buffer").Buffer;
const path = require('path');
const robot = require("robotjs");
const Jimp = require('jimp');
const PNG = require('pngjs').PNG;
const pixelmatch = require('pixelmatch');
const { imprintParts, imprintStart, imprintComplete } = require('./kek');
const directoryPath = path.join(__dirname, 'Documents');
var util = require('util');

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow

function createWindow() {
  // Create the browser window.
  mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js')
    }
  });

  // and load the index.html of the app.
  mainWindow.loadFile('index.html');

  // Open the DevTools.
  // mainWindow.webContents.openDevTools()

  // Emitted when the window is closed.
  mainWindow.on('closed', function () {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null
  });
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', createWindow);
app.on('ready', () => {
  const mainImprintXPos = 1118;
  const mainImprintWidth = 44;
  const mainImprintYPos = 164;
  const imprintPartStartXPos = 480;
  const imprintPartStartYPos = 278;
  const imprintPartNextPos = 144;
  const imprintPartWidth = 106;
  const imprintPartHeight = 3;
  robot.setKeyboardDelay(30);
  robot.setMouseDelay(30);
  const findBestWay = (coords, keys, i) => {
    let newKeys = [];
    coords.reduce((currentPos, targetPos) => {
      let distanceGoBottom = targetPos - currentPos;
      if (distanceGoBottom < 0) distanceGoBottom += 8;
      const distanceGoTop = 8 - targetPos + currentPos;
      const goTo = distanceGoTop > distanceGoBottom ?
        {
          x: 'right',
          y: 'down',
          xCount: distanceGoBottom % 2,
          yCount: Math.floor(distanceGoBottom / 2)
        } : {
          x: 'left',
          y: 'up',
          xCount: distanceGoTop % 2,
          yCount: Math.floor(distanceGoTop / 2)
        };
      newKeys.push(...Array(goTo.yCount).fill(goTo.y));
      if (goTo.xCount !== 0) newKeys.push(goTo.x);
      newKeys.push("click");
      return targetPos;
    }, 0);
    if (i !== 0 && newKeys.length > keys.length) newKeys = keys;
    if (newKeys.length < 9 || i === 7) return newKeys;
    const nextCoords = [...coords];
    if (i % 2 === 0 || i === 3) {
      [nextCoords[2], nextCoords[3]] = [nextCoords[3], nextCoords[2]];
      if (i === 3) [nextCoords[0], nextCoords[1]] = [nextCoords[1], nextCoords[0]];
    }
    else[nextCoords[1], nextCoords[2]] = [nextCoords[2], nextCoords[1]];
    return findBestWay(nextCoords, newKeys, i + 1);
  }
  let imprintMaker = null;
  let isOn = false;
  let win = new BrowserWindow({ frame: false, backgroundColor: '#50C878', width: 5, height: 5, x: 1860, y: 25, focusable: false, alwaysOnTop: true });
  win.hide();
  globalShortcut.register('numadd', () => {
    isOn = !isOn;
    if (isOn) {
      win.showInactive();
      win.setAlwaysOnTop(true, 'screen');
      imprintMaker = setInterval(() => {
        if (pixelmatch(Buffer.from(imprintComplete.data), robot.screen.capture(705, 517, 15, 4).image, null, 15, 4, { threshold: 0.2 }) / imprintPartWidth < 0.01) {
          isOn = false;
          clearInterval(imprintMaker);
          win.hide();
        }
        if (pixelmatch(Buffer.from(imprintStart.data), robot.screen.capture(460, 261, 6, 3).image, null, 6, 3, { threshold: 0.2 }) / imprintPartWidth < 0.01) {
          const mainImprint = robot.screen.capture(mainImprintXPos, mainImprintYPos, mainImprintWidth, 1);
          let imprintNumber = 0;
          for (; imprintNumber < mainImprintWidth; imprintNumber++)
            if (mainImprint.colorAt(imprintNumber, 0).indexOf('3a') !== -1)
              break;
          const coord = [];
          if (imprintParts[imprintNumber] !== undefined)
            for (let y = 0; y < 4; y++)
              for (let x = 0; x < 2; x++)
                if (imprintParts[imprintNumber].some(item =>
                  pixelmatch(
                    robot.screen.capture(imprintPartStartXPos + imprintPartNextPos * x, imprintPartStartYPos + imprintPartNextPos * y, imprintPartWidth, imprintPartHeight).image,
                    Buffer.from(item.data),
                    null,
                    imprintPartWidth,
                    imprintPartHeight,
                    { threshold: 0.2 }
                  ) / imprintPartWidth < 0.01
                ))
                  coord.push(y * 2 + x);
          findBestWay(coord, [], 0).forEach(action => {
            if (action === 'click') {
              robot.mouseToggle('up', 'left');
              robot.mouseToggle('down', 'left');
            }
            else robot.keyTap(action)
          });
          robot.keyTap("tab");
        }
      }, 100);
    } else {
      clearInterval(imprintMaker);
      win.hide();
    }
  });
});

app.on('will-quit', () => {
  globalShortcut.unregisterAll()
})

// Quit when all windows are closed.
app.on('window-all-closed', function () {
  // On macOS it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') app.quit();
})

app.on('activate', function () {
  // On macOS it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) createWindow();
})

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.
