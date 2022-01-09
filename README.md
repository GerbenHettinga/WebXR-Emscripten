## Dependencies 

- GLM: https://github.com/g-truc/glm
- emscripten: https://emscripten.org/
- emscripten-webxr: https://github.com/WonderlandEngine/emscripten-webxr (included as submodule)

## How to Build

- Create directory build
- cd build
- emcmake cmake ../
- emmake make

## How to Run

To run on a local machine:
- You have to serve it using https in order for the browser XR session to start
- I use browser-sync:
  npx browser-sync start --https --server




