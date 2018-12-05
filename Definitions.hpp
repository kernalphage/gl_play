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
#include <optional>

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



namespace DonerSerializer
{

	template <>
	class CDeserializationResolver::CDeserializationResolverType<glm::vec4>
	{
	public:
		static void Apply(glm::vec4& value, const rapidjson::Value& att)
		{
			if (att.IsArray())
			{
				value = glm::vec4{att[0].GetFloat(), att[1].GetFloat(), att[2].GetFloat(),att[3].GetFloat()};
			}
		}
	};

	template <>
	class CSerializationResolver::CSerializationResolverType<glm::vec4>
	{
	public:
		static void Apply(const char* name, const glm::vec4& value, rapidjson::Document& root)
		{
			rapidjson::Value array(rapidjson::kArrayType);
			CSerializationResolver::CSerializationResolverType<float>::SerializeToJsonArray(array, value.x, root.GetAllocator());
			CSerializationResolver::CSerializationResolverType<float>::SerializeToJsonArray(array, value.y, root.GetAllocator());
			CSerializationResolver::CSerializationResolverType<float>::SerializeToJsonArray(array, value.z, root.GetAllocator());
			CSerializationResolver::CSerializationResolverType<float>::SerializeToJsonArray(array, value.w, root.GetAllocator());
			root.AddMember(rapidjson::GenericStringRef<char>(name), array, root.GetAllocator());
		}

		static void SerializeToJsonArray(rapidjson::Value& root, const glm::vec4& value, rapidjson::Document::AllocatorType& allocator)
		{
			rapidjson::Value array(rapidjson::kArrayType);
			CSerializationResolver::CSerializationResolverType<float>::SerializeToJsonArray(array, value.x, allocator);
			CSerializationResolver::CSerializationResolverType<float>::SerializeToJsonArray(array, value.y, allocator);
			CSerializationResolver::CSerializationResolverType<float>::SerializeToJsonArray(array, value.z, allocator);
			CSerializationResolver::CSerializationResolverType<float>::SerializeToJsonArray(array, value.w, allocator);
			root.PushBack(array, allocator);
		}
	};
}


struct rect{
  vec2 tl;
  vec2 br;
};

struct UI_Vertex {
  vec3 pos;
  vec4 color = {1.0f, 0.0f, 1.0f, 1.0f};
};
struct Particle_Vertex{
  vec3 pos;
	struct particle_data{
	  	vec2 uv;
	  	float texSize;
	  	float worldSize;
	};
	particle_data m_data;
};

struct Geo{

	// T should be a vec2, but i do a lot with vec3s just in case
	template<typename T>
	static float TriSign(T a, T b, T c) {
		return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
	};
  // TODO: make bool
	template<typename T>
	static float line_intersect(T a, T b, T c, T d){
		float a1 = TriSign(a,b,d);
		float a2 = TriSign(a,b,c);
		if (a1*a2 < 0.0){
			float a3 = TriSign(c,d,a);
			float a4 = a3+a2-a1;
			if(a3*a4 < 0.0){
				float t = a3 / (a3 - a4);
				return t;
			}
		}
		return -1;
	}

	static vec3 fibonacci(const int k, const int n){
      // https://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere
      float golden_angle = 3.1415f * (3 - sqrt(5.f));

      float r = sqrt(1.0f * k) / sqrt(1.0f * n);
      float phi = k * golden_angle;  // = ((i + rnd) % samples) * increment
      float x = cos(phi)*r;
      float y = sin(phi)*r;
      return vec3{x,y,0};
    };
};

struct Util{
  static float rangeMap (float t,float inStart,float inEnd,float outStart, float outEnd, bool doClamp = false){
    float out = t - inStart;
    out /= (inEnd - inStart); // [0,1]
		// TODO: clamp/warp/reflect enum?
		if(doClamp){
			out = glm::clamp(out, 0.0f,1.0f);
		}
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

#include "Processing.hpp"

#endif // DEFINITIONS_HPP