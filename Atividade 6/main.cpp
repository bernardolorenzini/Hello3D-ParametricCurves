//
//  main.cpp
//  Atividade 5
//
//  Created by Bernardo Lorenzini on 30/05/24.
//

#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>
#include <fstream>
#include <sstream>
#include "cam.h"

#include "Hermite.h"
#include "CatmullRom.h"

#include "Bezier.h"


using namespace std;

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#include "Shader.h"
#include <thread>

const float Pi = 3.141;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Header das funÁıes
void obj_reader(string path);
void mtl_reader(string path);
int setupSprite();
int loadTexture(string path);


const GLuint WIDTH = 850, HEIGHT = 650;
bool rotateX = false, rotateY = false, rotateZ = false;

std::vector< GLfloat > triangles = {};
vector<GLfloat> ka;
vector<GLfloat> ks;
float ns;
string MTLname = "";
string TexturePath = "";

cam cam;

vector <glm::vec3> generateControlPointsSet(string path);



int main()
{
    glfwInit();



    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "atividade 5 - bernardo Lorenzini", nullptr, nullptr);
    glfwMakeContextCurrent(window);


    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Obtem as informaÁıes de vers„o
    const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
    const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    Shader shader("../Shaders/sprite.vs", "../Shaders/sprite.fs");

    obj_reader("../Objects/SuzanneTriTextured.obj");

    mtl_reader("../Objects/" + MTLname);

    GLuint texID = loadTexture("../Textures/" + TexturePath);

    // Gera uma geometria de quadrado com coordenadas de textura
    GLuint VAO = setupSprite();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Ativa o shader
    glUseProgram(shader.ID);

    cam.initialize(&shader, WIDTH, HEIGHT);


    shader.setVec3("ka", ka[0], ka[1], ka[2]);
    shader.setFloat("kd", 2.0);
    shader.setVec3("ks", ks[0], ks[1], ks[2]);
    shader.setFloat("q", ns);

    shader.setVec3("lightPos", -5.0f, 200.0f, 5.0f);
    shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

    std::vector<glm::vec3> CP = generateControlPointsSet("../curva/pontos.txt");

    Bezier bezier;
    bezier.setControlPoints(CP);
    bezier.setShader(&shader);
    bezier.generateCurve(100);
    int nbCurvePoints = bezier.getNbCurvePoints();
    int i = 0;

    glEnable(GL_DEPTH_TEST);

    float previous = glfwGetTime();
    float current = 0;
    
    const float updateInverval = 0.01;
    // Loop da aplicaÁ„o
    while (!glfwWindowShouldClose(window))
    {
        // Checa se houveram eventos de input (teclado, mouse) e chama as funÁıes de callback correspondentes
        glfwPollEvents();

        // Define as dimensıes da viewport com as mesmas dimensıes da janela da aplicaÁ„o
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        // Limpa o buffer de cor
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f); //cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glLineWidth(10);
        glPointSize(20);


        float current = glfwGetTime();
        if ((current - previous) > updateInverval) {
            i = (i + 1) % nbCurvePoints;
            previous = current;
        }


        //Cria a matriz de modelo usando o GLM
        glm::mat4 model = glm::mat4(1); //matriz identidade
        float angle = (GLfloat)glfwGetTime() / 10;

        glm::vec3 pointOnCurve = bezier.getPointOnCurve(i);
        model = glm::translate(model, glm::vec3(pointOnCurve));
        if (rotateX)
        {
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        else if (rotateY)
        {
            model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else if (rotateZ)
        {
            model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
        }
        model = glm::scale(model, glm::vec3(1.0, 1.0, 1.0));

        GLint modelLoc = glGetUniformLocation(shader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(model));

        cam.update();

        //Ativa o primeiro buffer de textura (0) e conectando ao identificador gerado
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, (triangles.size() / 8));

        glBindVertexArray(0); //unbind

        glBindTexture(GL_TEXTURE_2D, 0); //unbind da textura



        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }
    // desaloca os buffers
    glDeleteVertexArrays(1, &VAO);
    // Finaliza a execuÁ„o da GLFW e limpando os recursos alocados
    glfwTerminate();
    return 0;
}

// FunÁ„o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        rotateX = 1;
        rotateY = 0;
        rotateZ = 0;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        rotateX = 0;
        
        rotateY = 1;
        
        rotateZ = 0;
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        rotateX = 0;
        rotateY = 0;
        rotateZ = 1;
    }
    cam.move(window, key, action);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    cam.rotate(window, xpos, ypos);
}


void obj_reader(string path) {

    std::vector< glm::vec3 > indice_de_vertices = {};
    std::vector< glm::vec2 > indice_de_textura = {};
    std::vector< glm::vec3 > indice_aux = {};

    
    string line, v, t, valuesX, valuesY, valuesZ;
    int i = 0;
    ifstream myfile(path);
    while (!myfile.eof()) {

        get_line(myfile, line);

        std::istringstream iss(line);

        if (line.find("mtllib") == 0) {
            iss >> v >> MTLname;
        }

        if (line.find("v ") == 0) {
            iss >> v >> valuesX >> valuesY >> valuesZ;
            glm::vec3 aux = { std::stof(valuesX), std::stof(valuesY), std::stof(valuesZ) };
            indice_de_vertices.push_back(aux);
        }

        if (line.find("vn") == 0) {
            iss >> v >> valuesX >> valuesY >> valuesZ;
            glm::vec3 aux = { std::stof(valuesX), std::stof(valuesY), std::stof(valuesZ) };
            indice_aux.push_back(aux);
        }

        if (line.find("vt") == 0) {
            iss >> v >> valuesX >> valuesY;
            glm::vec2 aux = { std::stof(valuesX), std::stof(valuesY) };
            indice_de_textura.push_back(aux);
        }

        if (line.find("f ") == 0)
        {
            string delimiter = " ";
            string d1 = "/";

            //pos vertices
            int vertex_pos[] = { 0, 0, 0 };
            //pos texturas
            int textures_pos[] = { 0, 0 , 0 };
            //pos normal
            int normais_pos[] = { 0, 0, 0 };
            int finish = 0;
            for (int i = 0; i < 3; i++) {
                finish = line.find(delimiter); 
                
                
                line = line.substr(finish + 1);
                if (finish != line.npos) { 
                    
                    
                    vertex_pos[i] = stoi(line.substr(0, line.find(d1)));
                    finish = line.find(d1);
                    
                    line = line.substr(finish + 1);
                    textures_pos[i] = stoi(line.substr(0, line.find(d1)));
                    finish = line.find(d1); 
                    
                    line = line.substr(finish + 1);
                    normais_pos[i] = stoi(line.substr(0, line.find(d1)));

                    triangles.push_back(indice_de_vertices[vertex_pos[i] - 1][0]);
                    
                    triangles.push_back(indice_de_vertices[vertex_pos[i] - 1][1]);
                    
                    triangles.push_back(indice_de_vertices[vertex_pos[i] - 1][2]);
                    
                    
                    triangles.push_back(indice_de_textura[textures_pos[i] - 1][0]);
                    
                    triangles.push_back(indice_de_textura[textures_pos[i] - 1][1]);
                    
                    triangles.push_back(indice_aux[normais_pos[i] - 1][0]);
                    
                    triangles.push_back(indice_aux[normais_pos[i] - 1][1]);
                    
                    triangles.push_back(indice_aux[normais_pos[i] - 1][2]);
                    
                }
            }
        }
    }
    myfile.close();
}
void mtl_reader(string path) {
    //leitura do arquivo
    string line, v;
    ifstream myfile(path);

    while (!myfile.eof()) {
        get_line(myfile, line);

        std::istringstream iss(line);

        if (line.find("map_Kd") == 0) {
            iss >> v >> TexturePath;
        }
        else if (line.find("Ka") == 0)
        {
            float ka1, ka2, ka3;
            iss >> v >> ka1 >> ka2 >> ka3;
            ka.push_back(ka1);
            ka.push_back(ka2);
            ka.push_back(ka3);
        }
        else if (line.find("Ks") == 0)
        {
            float ks1, ks2, ks3;
            iss >> v >> ks1 >> ks2 >> ks3;
            ks.push_back(ks1);
            ks.push_back(ks2);
            ks.push_back(ks3);
        }
        else if (line.find("Ns") == 0)
        {
            iss >> v >> ns;
        }
    }
    myfile.close();
}

int setupSprite()
{
    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, triangles.size() * (sizeof(GLfloat)), triangles.data(), GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}

int loadTexture(string path)
{
    GLuint texID;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}
vector<glm::vec3> generateControlPointsSet(string path)
{
    vector <glm::vec3> aux;
    string line;
    ifstream config_File(path);
    while (get_line(config_File, line))
    {
        istringstream iss(line);
        float x, y, z;
        iss >> x >> y >> z;
        aux.push_back(glm::vec3(x, y, z));
    }
    config_File.close();
    return aux;
}
