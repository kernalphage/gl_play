Ideas for shader inlining: 

#ifdef debug
	int shader_src(string shadername){
		// ... load from file ... 
	}
#else
	int shader_src(string shadername){
		return shader_map[shadername];
	}
#endif 

this way i get both the benefits of quick reload in debug time and portability at runtime.
using: 
In your program, put the shader in:

const char shader_code = {
#include "shader_code.data"
, 0x00};
In shader_code.data there should be the shader source code as a list o hex numbers separated by commas. These files should be created before compilation using your shader code written normally in a file. In Linux I would put instructions at Makefile to run the code:

cat shader_code.glsl | xxd -i > shader_code.data
(but since I'm using cmake it'd be add_custom_command()) 