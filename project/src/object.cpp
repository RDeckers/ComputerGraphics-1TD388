#include <object.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <string.h>
#include <errno.h>
#include <utilities/logging.h>
#include <utilities/rng.h>

#include <string>
#include <fstream>
#include <sstream>

Object::Object(const char *obj_file){
  GLenum err = glGetError();
  report(INFO, "before instantion: %s (%d)", glewGetErrorString(err), err);

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(2, m_vbo);
  load_file(obj_file);
}

Object::~Object()
{
  glDeleteBuffers(2, m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void Object::populate_gl_buffers(){
  GLenum error = glGetError();
  report(INFO, "before population: %s (%d)", glewGetErrorString(error), error);
  glBindVertexArray(m_vao);

  size_t normals_size = m_normals.size()*sizeof(m_normals[0]),
       vertices_size = m_vertices.size()*sizeof(m_vertices[0]),
       colors_size = m_colors.size()*sizeof(m_colors[0]),
      matrix_size = m_model_matrices.size()*(16*sizeof(GLfloat));
  size_t offset = 0;
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
  glBufferData(
        GL_ARRAY_BUFFER,
        normals_size+vertices_size+colors_size+matrix_size,
       nullptr, GL_STATIC_DRAW);
  
  glBufferSubData(
        GL_ARRAY_BUFFER,
        offset,
        vertices_size,
        &(m_vertices[0])
      );
  offset += vertices_size;
  
  glBufferSubData(
      GL_ARRAY_BUFFER,
      offset,
      normals_size,
      &(m_normals[0])
  );
  offset += normals_size;
  
  glBufferSubData(
    GL_ARRAY_BUFFER,
    offset,
    colors_size,
     &(m_colors[0])
  );
  offset += colors_size;
  
  glBufferSubData(
    GL_ARRAY_BUFFER,
    offset,
    matrix_size,
      &(m_model_matrices[0])
  );

  glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(POSITION);
  
  glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (void*)vertices_size);
  glEnableVertexAttribArray(NORMAL);
  
  glVertexAttribPointer(COLOR, 4, GL_FLOAT, GL_FALSE, 0, 
                        (void*)(vertices_size+normals_size));
  glEnableVertexAttribArray(COLOR);
  
  glEnableVertexAttribArray(MODEL_MATRIX+0);
  glEnableVertexAttribArray(MODEL_MATRIX+1);
  glEnableVertexAttribArray(MODEL_MATRIX+2);
  glEnableVertexAttribArray(MODEL_MATRIX+3);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glEnableVertexAttribArray: %s (%d)", glewGetErrorString(error), error);
  size_t base_matrix_offset = vertices_size+normals_size+colors_size;
  glVertexAttribPointer(
        MODEL_MATRIX+0,4, GL_FLOAT, GL_FALSE, 16*sizeof(GLfloat),
        (void*)(base_matrix_offset+0*sizeof(GLfloat)));
  glVertexAttribPointer(
        MODEL_MATRIX+1, 4, GL_FLOAT, GL_FALSE, 16*sizeof(GLfloat),
        (void*)(base_matrix_offset+4*sizeof(GLfloat)));
  glVertexAttribPointer(
        MODEL_MATRIX+2, 4, GL_FLOAT, GL_FALSE, 16*sizeof(GLfloat),
        (void*)(base_matrix_offset+8*sizeof(GLfloat)));
  glVertexAttribPointer
      (MODEL_MATRIX+3, 4, GL_FLOAT, GL_FALSE, 16*sizeof(GLfloat),
       (void*)(base_matrix_offset+12*sizeof(GLfloat)));
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glVertexAttribPointer: %s (%d)", glewGetErrorString(error), error);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(m_indices[0]), m_indices.data(), GL_STATIC_DRAW);
  glVertexAttribDivisor(COLOR, 1);
  glVertexAttribDivisor(MODEL_MATRIX+0, 1);
  glVertexAttribDivisor(MODEL_MATRIX+1, 1);
  glVertexAttribDivisor(MODEL_MATRIX+2, 1);
  glVertexAttribDivisor(MODEL_MATRIX+3, 1);
  error = glGetError();
  report(INFO, "after population: %s (%d)", glewGetErrorString(error), error);
}

glm::mat4 Object::model_matrix(){
  return glm::translate(m_translation)*glm::yawPitchRoll(fast_rngf(0,6.28), fast_rngf(0,6.28), fast_rngf(0,6.28))*glm::scale(m_scale);
  //return glm::scale(glm::vec3(0.1f));
  //return glm::mat4();
}

bool Object::load_file(const char *obj_file)
{
  GLenum err = glGetError();
  report(INFO, "before load: %s (%d)", glewGetErrorString(err), err);
  const std::string VERTEX_LINE("v ");
  const std::string FACE_LINE("f ");

  // Open OBJ file
  std::ifstream f(obj_file);
  if (!f.is_open()) {
    report(FAIL, "Could not open %s: %s (%d)", obj_file, strerror(errno), errno);
    return false;
  }

  // Extract vertices and indices
  std::string line;
  glm::vec3 vertex;
  std::uint32_t vertexIndex0, vertexIndex1, vertexIndex2;
  while (!f.eof()) {
    std::getline(f, line);
    if (line.substr(0, 2) == VERTEX_LINE) {
      std::istringstream vertexLine(line.substr(2));
      vertexLine >> vertex.x;
      vertexLine >> vertex.y;
      vertexLine >> vertex.z;
      m_vertices.push_back(vertex);
    }
    else if (line.substr(0, 2) == FACE_LINE) {
      std::istringstream faceLine(line.substr(2));
      faceLine >> vertexIndex0;
      faceLine >> vertexIndex1;
      faceLine >> vertexIndex2;
      m_indices.push_back(vertexIndex0 - 1);
      m_indices.push_back(vertexIndex1 - 1);
      m_indices.push_back(vertexIndex2 - 1);
    }
    else {
      // Ignore line
    }
  }

  // Close OBJ file
  f.close();

  // Compute normals
  this->compute_normals();
  for(unsigned u = 0; u < m_instance_count; u++){
    m_model_matrices.push_back(glm::translate(glm::vec3(fast_rngf(-3, 3), fast_rngf(-3, 3), fast_rngf(-3, 3)))*glm::yawPitchRoll(fast_rngf(0,6.28), fast_rngf(0,6.28), fast_rngf(0,6.28))*glm::scale(glm::vec3(fast_rngf(0.1, 1))));
    m_colors.push_back(glm::vec4(fast_rngf(1,1), fast_rngf(1,1), fast_rngf(1,1), fast_rngf(1,32)));
  }
  this->populate_gl_buffers();
  // Display log message
  unsigned numTriangles = m_indices.size() / 3;
  report(PASS, "Loaded file %s, contain %u triangles", obj_file, numTriangles);
  return true;
}

void Object::compute_normals(){
  GLenum err = glGetError();
  report(INFO, "before normal computation: %s (%d)", glewGetErrorString(err), err);
  int numNormals = m_vertices.size();
  m_normals.resize(numNormals);

  // Compute per-vertex normals by averaging the unnormalized face normals
  std::uint32_t vertexIndex0, vertexIndex1, vertexIndex2;
  glm::vec3 normal;
  int numIndices = m_indices.size();
  for (int i = 0; i < numIndices; i += 3) {
    vertexIndex0 = m_indices[i];
    vertexIndex1 = m_indices[i + 1];
    vertexIndex2 = m_indices[i + 2];
    normal = glm::cross(m_vertices[vertexIndex1] - m_vertices[vertexIndex0],
                        m_vertices[vertexIndex2] - m_vertices[vertexIndex0]);
    m_normals[vertexIndex0] += normal;
    m_normals[vertexIndex1] += normal;
    m_normals[vertexIndex2] += normal;
  }


  for (int i = 0; i < numNormals; i++) {
    m_normals[i] = glm::normalize(m_normals[i]);
  }
}

void Object::draw(){
  glBindVertexArray(m_vao);
  glDrawElementsInstanced(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0, m_instance_count);
}
