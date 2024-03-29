#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const char* vertShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aColor;\n"
"out vec3 vertexColor;\n"
"void main() {"
"	gl_Position = vec4(aPos, 1.0);\n"
"	vertexColor = aColor;\n"
"}\0";

const char* fragShaderSource =
"#version 330 core\n"
"in vec3 vertexColor;\n"
"out vec4 FragColor;\n"
"void main() {"
"	gl_FragColor = vec4(vertexColor, 1.0);\n"
"}\0";

void window_resize_callback(GLFWwindow* window, int width, int height) {
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
	GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Playground", nullptr, nullptr);
	if (!window) {
		spdlog::error("Failed to create window");
		spdlog::info("Initiate cleanup");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetWindowSizeCallback(window, window_resize_callback);
	glfwMakeContextCurrent(window);

	spdlog::info("Load OpenGL function pointers");
	if (!gladLoadGL()) {
		spdlog::error("Failed to load OpenGL function pointers");
		spdlog::info("Initiate cleanup");
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	GLfloat vertices[] = {
		-0.5f, -0.433f, 0.0f,
		1.0f, 0.0f, 0.0f,

		0.5f, -0.433f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, 0.433f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};

	spdlog::info("Setup graphics pipeline");

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

	spdlog::info("Setup VAO and VBO");
	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	spdlog::info("Start window event loop");
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProg);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	spdlog::info("Initiate cleanup");
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}