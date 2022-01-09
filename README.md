## Dependencies 

- GLM: https://github.com/g-truc/glm
- emscripten: https://emscripten.org/
- emscripten-webxr: https://github.com/WonderlandEngine/emscripten-webxr (included as submodule)

## How to Build

- mkdir build
- cd build
- emcmake cmake ../
- emmake make

## How to Run

To run on a local machine:
- You have to serve it using https in order for the browser XR session to start
- I use browser-sync from npm:
  npx browser-sync start --https --server

## Enjoy
![example](https://user-images.githubusercontent.com/6782231/148680173-1066a5a8-b859-41f1-879c-d0762936af4e.png)


