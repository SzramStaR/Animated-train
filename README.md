# OpenGL Project

This project is an OpenGL program written in C++ that demonstrates various features of OpenGL, including 3D rendering, texturing, and shader programming. It displays a 3D train model with rotating wheels, along with a grassy plane and a background plane.

## Installation

To run the program, follow these steps:

1. Make sure you have the necessary dependencies installed:
   - GLEW
   - GLFW
   - GLM
   - LodePNG

2. Clone the project repository to your local machine.

3. Compile the source code using a C++ compiler, ensuring that the necessary libraries are linked.

4. Run the executable file generated after compilation.

## Usage

Once the program is running, you can interact with the scene using keyboard inputs. The following controls are available:

- Arrow keys: Rotate the view angle.

## License

This program is distributed under the GNU General Public License version 3 or later. See the [LICENSE](LICENSE) file for more details.

## Credits

The program includes the following files:

- `constants.h`: Contains constant values used in the program.
- `lodepng.h`: Provides functions for reading PNG images.
- `shaderprogram.h`: Defines a shader program class for loading and using shaders.
- `myTeapot.h`: Contains vertex data for rendering a teapot.
- `fast_obj.h`: Header file for a fast OBJ file loader.
- `v_simplest.glsl` and `f_simplest.glsl`: Vertex and fragment shader programs for the train model.
- `v_smoke2.glsl`, `g_smoke.glsl`, and `f_smoke2.glsl`: Vertex, geometry, and fragment shader programs for the smoke effect.
- `v_grass.glsl` and `f_grass.glsl`: Vertex and fragment shader programs for the grass plane.
- `v_background.glsl` and `f_background.glsl`: Vertex and fragment shader programs for the background plane.
- `steel.png`, `Solid_white.png`, `sky.png`, `grass_tex.png`, and `fur.png`: Texture images used in the program.

The train and wheel models used in the program are provided in the `wheeltest.obj`, `cloud.obj`, `train_bez.obj`, and `wheelsmallmodel.obj` files.

The program was developed using the OpenGL library and the following C++ libraries:

- GLEW (OpenGL Extension Wrangler Library)
- GLFW (OpenGL framework)
- GLM (OpenGL Mathematics)
- LodePNG (PNG image decoder and encoder)


## Acknowledgments

This project was inspired by the need to demonstrate various features and techniques of OpenGL. Special thanks to the authors of the libraries and resources used in the development of this program.
