#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP
#define NOMINMAX


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/ext.hpp>
#include <string>
#include <ctime>
#include <donerserializer/DonerSerialize.h>
#include <donerserializer/DonerDeserialize.h>
#include <sstream>
#include <iostream>

class Processing;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;


#define SPLAT2(t) t.x, t.y
#define SPLAT3(t) t.x, t.y, t.z
#define SPLAT4(t) t.x, t.y, t.z, t.w

#define SPLAT_OP_2(t, op) t.x op t.y
#define SPLAT_OP_3(t, op) t.x op t.y op t.z
#define SPLAT_OP_4(t, op) t.x op t.y op t.z op t.w

#define impl_STATIC_DO_ONCE(x, y) {static bool __doonce##y=true; if(__doonce##y == true){__doonce##y = false; x;}};
#define STATIC_DO_ONCE(x) impl_STATIC_DO_ONCE(x, __LINE__);


struct rect{
  vec2 tl;
  vec2 br;
};

struct UI_Vertex {
  vec3 pos;
  vec4 color = {1.0f, 0.0f, 1.0f, 1.0f};
};
struct TEX_Vertex{
  vec3 pos;
  vec4 color;
  vec2 uv;
};



struct Util{
  static float rangeMap (float t,float inStart,float inEnd,float outStart, float outEnd){
    //if(! easing ) easing = x=>x;
    float out = t - inStart;
    out /= (inEnd - inStart); // [0,1]
    //out = easing(out);
    out *= (outEnd - outStart);
    return out + outStart;
  }
static	std::string timestamp(int seed){
	  char mbstr[150];
	   std::time_t t = std::time(nullptr);
	  std::strftime(mbstr, sizeof(mbstr), "%m_%d_%s_%%d.txt", std::localtime(&t));
	  sprintf(mbstr, mbstr, seed);
	  return std::string(mbstr);
	}


	template<typename T>
	static bool load_json(T& obj, const char* filename, bool doSave, bool doLoad){
		if(doSave){
			std::ofstream settings(filename);

			DonerSerializer::CJsonSerializer serializer;
			serializer.Serialize(obj);
			std::string result = serializer.GetJsonString();
			settings<<result;
		}

		if(doLoad){
			std::ifstream sFile(filename);
			std::stringstream vShaderStream;

			if(!sFile.is_open()){
				return false;
			}
			vShaderStream << sFile.rdbuf();
			sFile.close();
			// catch

			std::string jsonContents=vShaderStream.str();
			std::cout<<jsonContents<<std::endl;
			DonerSerializer::CJsonDeserializer::Deserialize(obj, jsonContents.c_str());
		}
		return doSave || doLoad;
	}
};


#endif // DEFINITIONS_HPP