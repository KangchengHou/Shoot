#ifndef BOX_RENDERER_H
#define BOX_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "texture.h"
#include "Shader.h"


class BoxRenderer
{
public:
    // Constructor (inits shaders/shapes)
    BoxRenderer(Shader shader);
    // Destructor
    ~BoxRenderer();
    // Renders a defined quad textured with given Box
    void DrawBox(Texture2D texture0, Texture2D texture1, glm::vec3 , glm::vec3 size, glm::vec3 color, Camera& camera);
private:
    // Render state
    Shader shader; 
    GLuint containerVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};

#endif