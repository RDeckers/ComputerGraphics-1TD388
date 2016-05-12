#include <object.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string.h>
#include <errno.h>
#include <utilities/logging.h>

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
  GLenum err = glGetError();
  report(INFO, "before population: %s (%d)", glewGetErrorString(err), err);
  glBindVertexArray(m_vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*(sizeof(m_vertices[0])+sizeof(m_normals[0])), nullptr, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size()*sizeof(m_vertices[0]), &(m_vertices[0]));
  glBufferSubData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(m_vertices[0]), m_normals.size()*sizeof(m_normals[0]), &(m_normals[0]));
  
  glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(POSITION);
  glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (void*)(m_vertices.size()*sizeof(m_vertices[0])));
  glEnableVertexAttribArray(NORMAL);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(m_indices[0]), m_indices.data(), GL_STATIC_DRAW);
  
  err = glGetError();
  report(INFO, "after population: %s (%d)", glewGetErrorString(err), err);
}

glm::mat4 Object::model_matrix(){
  return glm::translate(m_translation)*glm::mat4_cast(m_rotation)*glm::scale(m_scale);
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
  glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(this->model_matrix()));
  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}
