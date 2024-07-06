#include "Hermite.h"

Hermite::Hermite()
{
	Mat = glm::mat4(2, -2, 1, 1,
				  -3, 3, -2, -1,
				  0, 0, 1, 0,
				  1, 0, 0, 0		
	);
}

void Hermite::generateCurve(int pontos_de_seguimentos)
{
	float step = 1.0 / (float)pontos_por_segmentos;

	float t = 0;

	int ncontrole_de_pontos = controle_de_pontos.size();

	for (int i = 0; i < ncontrole_de_pontos - 3; i += 3)
	{
		
		for (float t = 0.0; t <= 1.0; t += step)
		{
			glm::vec3 p;

			glm::vec4 T(t * t * t, t * t, t, 1);

			glm::vec3 P0 = controle_de_pontos[i];
			glm::vec3 P1 = controle_de_pontos[i + 3];
			glm::vec3 T0 = controle_de_pontos[i + 1] - P0;
			glm::vec3 T1 = controle_de_pontos[i + 2] - P1;

			glm::mat4x3 G(P0, P1, T0, T1);

			p = G * Mat* T; 

			pontos_de_curvas.push_back(p);
		}
	}

	GLuint VBO;

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, pontos_de_curvas.size() * sizeof(GLfloat) * 3, pontos_de_curvas.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
 
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}
