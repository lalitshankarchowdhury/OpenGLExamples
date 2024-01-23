#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const char* vertShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
out vec3 vertColor;

void main() {
	gl_Position = vec4(aPos, 1.0);
	vertColor = aColor;
}
)";

const char* fragShaderSource = R"(
#version 330 core

in vec3 vertColor;
out vec4 FragColor;

void main() {
	FragColor = vec4(vertColor, 1.0);
}
)";

void window_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	spdlog::info("Window resized to dimensions: {:d}x{:d}", width, height);
}

int main() {
	spdlog::info("Initialize GLFW");
	if (!glfwInit()) {
		spdlog::error("Failed to initialize GLFW");
		return EXIT_FAILURE;
	}

	spdlog::info("Set OpenGL 3.3 core profile");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	spdlog::info("Create window: 640x480");
	GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL playground", nullptr, nullptr);
	if (!window) {
		spdlog::error("Failed to create window");
		spdlog::info("Initiate cleanup");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwMakeContextCurrent(window);

	spdlog::info("Load OpenGL function pointers");
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		spdlog::error("Failed to load OpenGL function pointers");
		spdlog::info("Initiate cleanup");
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	GLfloat vertices[][2][3] = {
		{{0.0, 0.0, 0.0},
		{1.0, 0.0, 0.0}},

		{{-0.75, -0.75, 0.0},
		{0.0, 1.0, 0.0}},

		{{-0.75, 0.75, 0.0},
		{0.0, 0.0, 1.0}},

		{{0.75, -0.75, 0.0},
		{0.0, 1.0, 0.0}},

		{{0.75, 0.75, 0.0},
		{0.0, 0.0, 1.0}},

		{{-0.1875, 0.0, 0.0},
		{0.0, 1.0, 0.0}},

		{{-0.625, -0.375, 0.0},
		{0.0, 0.0, 1.0}},

		{{-0.625, 0.375, 0.0},
		{1.0, 0.0, 0.0}},

		{{0.1875, 0.0, 0.0},
		{0.0, 1.0, 0.0}},

		{{0.625, -0.375, 0.0},
		{0.0, 0.0, 1.0}},

		{{0.625, 0.375, 0.0},
		{1.0, 0.0, 0.0}},
	};

	GLuint indices[] = {
		0, 1, 2,
		0, 3, 4,
		5, 6, 7,
		8, 9, 10
	};

	spdlog::info("Initialize graphics pipeline");

	spdlog::info("Process vertex and fragment shaders");
	GLuint vertShader, fragShader, shaderProg;

	vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertShaderSource, nullptr);
	glCompileShader(vertShader);

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragShaderSource, nullptr);
	glCompileShader(fragShader);

	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vertShader);
	glAttachShader(shaderProg, fragShader);
	glLinkProgram(shaderProg);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	spdlog::info("Process VAO, VBO, and IBO");
	GLuint VAO, VBO, IBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	spdlog::info("Begin window event loop");
	double frameTime = 1.0 / 60.0, prevTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		double currTime = glfwGetTime();
		if ((currTime - prevTime) < frameTime) {
			continue;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProg);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();

		prevTime = currTime;
	}

	spdlog::info("Initiate cleanup");
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteVertexArrays(1, &VAO);
	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}
