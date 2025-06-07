Needed bibariles
sudo apt install libstb-dev
sudo apt install libassimp-dev assimp-utils
sudo apt install libglm-dev
sudo apt-get install -y build-essential cmake
sudo apt-get install -y libglfw3-dev libglm-dev libassimp-dev
sudo apt-get install -y libstb-dev
sudo apt install pkgconf
sudo apt-get install libglfw3-dev libglm-dev libassimp-dev

mkdir build
cd build
cmake ..
make

./aquarium lub ./AquariumProject

├── resources/
│   ├── models/
│   │   ├── fish.obj
│   │   ├── coral.obj
│   │   ├── plant.obj
│   │   └── rock.obj
│   ├── shaders/
│   │   ├── fragment.glsl
│   │   └── vertex.glsl
│   └── textures/
│       ├── fish.png
│       ├── coral.jpg
│       ├── plant.png
│       ├── rock.jpg
│       └── water.png