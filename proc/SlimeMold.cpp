//
// Created by matt on 3/3/19.
//

#include "SlimeMold.hpp"
#include <iostream>
#include <Material.hpp>
#include "Random.hpp"

#define numFeedbacks 2

void SlimeMold::imSettings(bool redraw, bool clear) {

}

void SlimeMold::setup() {
  basic = new Material{"shaders/basic.vert", "shaders/basic.frag", true};

// Vertex shader
const GLchar* vertexShaderSrc = R"glsl(
    #version 150 core

    in vec2 position;
    in vec2 velocity;
    in vec2 originalPos;

    out vec2 outPosition;
    out vec2 outVelocity;
    out vec2 outoriginalPos;

    uniform float time;
    uniform vec2 mousePos;

    void main()
    {
        // Points move towards their original position...
        vec2 newVelocity = originalPos - position;

        // ... unless the mouse is nearby. In that case, they move towards the mouse.
        if (length(mousePos - originalPos) < 0.75f) {
            vec2 acceleration = 1.5f * normalize(mousePos - position);
            newVelocity = velocity + acceleration * time;
        }
        
        if (length(newVelocity) > 1.0f)
            newVelocity = normalize(newVelocity);

        vec2 newPosition = position + newVelocity * time;
        outPosition = newPosition;
        outVelocity = newVelocity;
        outoriginalPos =originalPos;
        gl_Position = vec4(newPosition, 0.0, 1.0);
    }
)glsl";

// Fragment shader
const GLchar* fragmentShaderSrc = R"glsl(
    #version 150 core

    out vec4 outColor;

    void main()
    {
        outColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
)glsl";

 // Compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vertexShader);
Material::checkCompileErrors(vertexShader, "VERT", "slime.vert");
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
    glCompileShader(fragmentShader);
Material::checkCompileErrors(fragmentShader, "frag", "frag.vert");

    // Create program and specify transform feedback variables
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    const GLchar* feedbackVaryings[] = { "outPosition", "outVelocity", "outoriginalPos" };
    glTransformFeedbackVaryings(program, 3, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

    glLinkProgram(program);
    glUseProgram(program);
  Material::checkCompileErrors(program, "PROGRAM", "frag.vert");
    uniTime = glGetUniformLocation(program, "time");
    uniMousePos = glGetUniformLocation(program, "mousePos");

    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create input VBO and vertex format
    
    // Vertex format: 6 floats per vertex:
    // pos.x  pox.y  vel.x  vel.y  origPos.x  origPos.y

    // Set original and initial positions
    float di = 1.0f / SLIME_ROWS;
    for (int y = 0; y < SLIME_ROWS; y++) {
        for (int x = 0; x < SLIME_ROWS; x++) {
            float dx = Random::f() - .5f;
            float dy = Random::f() - .5f;

            data[60 * y + 6 * x]     = dx;
            data[60 * y + 6 * x + 1] = dy;
            data[60 * y + 6 * x + 2] = 0;
            data[60 * y + 6 * x + 3] = 0;
            data[60 * y + 6 * x + 4] = dx;
            data[60 * y + 6 * x + 5] = dy;
        }
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STREAM_DRAW);

    GLint posAttrib = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

    GLint velAttrib = glGetAttribLocation(program, "velocity");
    glEnableVertexAttribArray(velAttrib);
    glVertexAttribPointer(velAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    GLint origPosAttrib = glGetAttribLocation(program, "originalPos");
    glEnableVertexAttribArray(origPosAttrib);
    glVertexAttribPointer(origPosAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(4 * sizeof(GLfloat)));

    // Create transform feedback buffer

    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_STREAM_DRAW);

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glPointSize(5.0f);

}
void SlimeMold::feedbackStep() {

   static auto t_prev = std::chrono::high_resolution_clock::now();
       

        // Calculate delta time
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_prev).count();
        t_prev = t_now;
        glUniform1f(uniTime, time);

        // Update mouse position
        static float t = 0;
        t += time;
        glUniform2f(uniMousePos, 0.0f, 0.0f);

        // Perform feedback transform and draw vertices
        glBeginTransformFeedback(GL_POINTS);
            glDrawArrays(GL_POINTS, 0, SLIME_NODES);
        glEndTransformFeedback();
        

    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glCopyBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, GL_ARRAY_BUFFER, 0,0, SLIME_TOTAL_SIZE);

}

void SlimeMold::render(Processing *ctx, bool &redraw, bool &clear, int curFrame, int maxFrames) {

}

void SlimeMold::setDrawArrays(){

  basic->use();
  glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex),
                        (void *)(sizeof(vec3)));
}
void SlimeMold::setVertexArrays(){
 /*   auto nodeSize= sizeof(SlimeNode);
  GLint inputAttrib = glGetAttribLocation(m_program, "i_position");
  glEnableVertexAttribArray(inputAttrib);
  glVertexAttribPointer(inputAttrib, 3, GL_FLOAT, GL_FALSE, nodeSize,0);

   inputAttrib = glGetAttribLocation(m_program, "i_color");
  glEnableVertexAttribArray(inputAttrib);
  glVertexAttribPointer(inputAttrib, 4, GL_FLOAT, GL_FALSE, nodeSize,(void*) sizeof(vec3));

   inputAttrib = glGetAttribLocation(m_program, "i_angle");
  glEnableVertexAttribArray(inputAttrib);
  glVertexAttribPointer(inputAttrib, 1, GL_FLOAT, GL_FALSE, nodeSize,(void*) (sizeof(vec4) + sizeof(vec3)));
*/
}
/*
void ParticleSystem::release()
{
    NvSafeDelete(m_feedbackProgram);
    NvSafeDelete(m_billboardProgram);
    NvSafeDelete(m_emitterProgram);
    
    glDeleteTextures(1,&m_randomTexture);
    glDeleteTextures(1,&m_particleTexture);
    glDeleteTextures(1,&m_FBMTexture);

    glDeleteTransformFeedbacks(countof(m_transformFeedback),m_transformFeedback);
    glDeleteBuffers(countof(m_particlesBuffers),m_particlesBuffers);
    glDeleteBuffers(1,&m_emittersBuffer);

    glDeleteQueries(countof(m_countQuery),m_countQuery);
}
*/