#include <cube.hpp>
#include <glm/gtc/type_ptr.hpp>

Cube::Cube(){
  const GLfloat vertices[3*6*6] = {
    //front
      -1,+1,+1,
      -1,-1,+1,
      +1,-1,+1,

      +1,+1,+1,
      -1,+1,+1,
      +1,-1,+1,
    //left
      -1,+1,-1,
      -1,-1,-1,
      -1,-1,+1,

      -1,+1,+1,
      -1,+1,-1,
      -1,-1,+1,
    //back
      +1,+1,-1,
      +1,-1,-1,
      -1,-1,-1,

      -1,+1,-1,
      +1,+1,-1,
      -1,-1,-1,
    //right
      +1,+1,+1,
      +1,-1,+1,
      +1,-1,-1,

      +1,+1,-1,
      +1,+1,+1,
      +1,-1,-1,

      //bottom
      -1,-1,+1,
      -1,-1,-1,
      +1,-1,-1,

      +1,-1,+1,
      -1,-1,+1,
      +1,-1,-1,
      //top
      -1,+1,-1,
      -1,+1,+1,
      +1,+1,+1,

      +1,+1,-1,
      -1,+1,-1,
      +1,+1,+1,
    };
  const GLfloat normals[3*6*6] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,

    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,

    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,

    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
  };
  const GLfloat colors[3*6*6] = {
    1.0, 1.0, 1.0,//white
    1.0, 1.0, 1.0,//white
    1.0, 1.0, 1.0,//white
    1.0, 1.0, 1.0,//white
    1.0, 1.0, 1.0,//white
    1.0, 1.0, 1.0,//white

    1.0, 0.0, 0.0,//red
    1.0, 0.0, 0.0,//red
    1.0, 0.0, 0.0,//red
    1.0, 0.0, 0.0,//red
    1.0, 0.0, 0.0,//red
    1.0, 0.0, 0.0,//red

    0.0, 1.0, 0.0,//green
    0.0, 1.0, 0.0,//green
    0.0, 1.0, 0.0,//green
    0.0, 1.0, 0.0,//green
    0.0, 1.0, 0.0,//green
    0.0, 1.0, 0.0,//green

    0.0, 0.0, 1.0,//blue
    0.0, 0.0, 1.0,//blue
    0.0, 0.0, 1.0,//blue
    0.0, 0.0, 1.0,//blue
    0.0, 0.0, 1.0,//blue
    0.0, 0.0, 1.0,//blue

    1.0, 1.0, 0.0,//yellow
    1.0, 1.0, 0.0,//yellow
    1.0, 1.0, 0.0,//yellow
    1.0, 1.0, 0.0,//yellow
    1.0, 1.0, 0.0,//yellow
    1.0, 1.0, 0.0,//yellow

    1.0, 0.0, 1.0,//magenta?
    1.0, 0.0, 1.0,//magenta?
    1.0, 0.0, 1.0,//magenta?
    1.0, 0.0, 1.0,//magenta?
    1.0, 0.0, 1.0,//magenta?
    1.0, 0.0, 1.0//magenta?
  };

  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(colors)+sizeof(normals), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(colors), sizeof(normals), normals);

  glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(POSITION);
  glVertexAttribPointer(COLOR, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(vertices));
  glEnableVertexAttribArray(COLOR);
  glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(vertices)+sizeof(colors)));
  glEnableVertexAttribArray(NORMAL);
}

Cube::~Cube(){
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

glm::mat4 Cube::model_matrix(){
  return glm::translate(m_translation)*glm::mat4_cast(m_rotation)*glm::scale(m_scale);
}

void Cube::draw(){
  glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(this->model_matrix()));
  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6*6);
}

void Cube::update(double global_time, double tick_time){
  rotate(glm::vec3(0.0,1,0), tick_time*(6.28/1000));
}

