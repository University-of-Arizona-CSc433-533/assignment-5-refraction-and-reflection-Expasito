#include <glad/glad.h>
#include <iostream>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <map>
#include <glm/gtx/string_cast.hpp>
#include <nlohmann/json.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <SOIL/stb_image.h>


// From parser.c
void loadScene(const char *path);

int width = 800;
int height = 800;


glm::vec3 eye;
glm::vec3 center;
glm::vec3 up;
float fov_angle;
float near;
float far;
glm::vec3 DefaultColor;
glm::vec3 SunLocation;

// Ordering of vectors: UL, LL, UR
std::vector<glm::vec3> billboard;
std::vector<glm::vec3> mirror;
std::string fileName; // For billboard
std::string objectName; // For object
glm::vec3 position; // For object



/*
    Simple function for loading a file. Used for loading shaders
    Input is a filepath and output is a char*.
    Limit is 5000 characters though this can be changed easily.
*/
char* readFile(const char* filename) {
    FILE *file = fopen(filename, "r");

    // Allocate our buffer
    char* data = (char*)malloc(sizeof(char) * 50000);
    int index = 0;

    // Read the char and put it on the buffer
    while(true) {
        char c;
        int err = fscanf(file, "%c", &c);
        if(err == -1){
            break;
        }
        data[index++] = c;
    }

    // Add a newline and null to terminate the string
    data[index++] = '\n';
    data[index++] = 0;

    return data;
}



// Bool for checking if the window should stay open or not
bool close = false;

// These will be my default values since they are not provided
float wavelength = 5.0f;
float amplitude = 1.0f;
float rr = 0.5f;

/*
    Simple function for resizing the window
*/
void frameBufferSizeCallBack(GLFWwindow *window, int width_, int height_) {
    width = width_;
    height = height_;
    glViewport(0,0,width,height);
}

// Bools for key presses
bool A = false;
bool W = false;
bool S = false;
bool D = false;
bool UP = false;
bool DOWN = false;
bool LEFT = false;
bool RIGHT = false;
bool SPACE = false;
bool SHIFT = false;

float milis = 0;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        close = true;
    }

    // Record all important key presses
    W = glfwGetKey(window, GLFW_KEY_W);
    A = glfwGetKey(window, GLFW_KEY_A);
    S = glfwGetKey(window, GLFW_KEY_S);
    D = glfwGetKey(window, GLFW_KEY_D);

    SPACE = glfwGetKey(window, GLFW_KEY_SPACE);
    SHIFT = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);

    UP = glfwGetKey(window, GLFW_KEY_UP);
    LEFT = glfwGetKey(window, GLFW_KEY_LEFT);
    DOWN = glfwGetKey(window, GLFW_KEY_DOWN);
    RIGHT = glfwGetKey(window, GLFW_KEY_RIGHT); 


}

// Keep track of if the d key has been pressed
int dPressed = false;

/*
    Function for updating camera position based on key presses
*/
void move() {
    // Get our lookat vector
    glm::vec3 lookat = glm::normalize(center - eye);

    // Scale milis down to be slower
    milis /= 100.0f;



    // Start the timer
	if (D) {
        dPressed = true;
	}

    // Adjust the reflected and refracted ratio
	if (SPACE) {
        rr += milis * .05;
	}
	if (SHIFT) {
        rr -= milis * .05;
	}



    // Adjust the wavelength and amplitude of the waves
	if(LEFT) {
        wavelength -= milis * .1f;
	}
	if(RIGHT) {
        wavelength += milis * .1f;
	}
	if(UP) {
        amplitude += milis * .1f;
	}
	if(DOWN) {
        amplitude -= milis * .1f;
	}
}

/*
    Small function to create a shader based on text and the type
    char **data is the string that holds the shader code

    GLenum shaderType is either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
        depending on its type
*/
uint32_t genShader(char **data, GLenum shaderType) {

    // Create our shader and read the code int
    uint32_t shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, data, NULL);
    glCompileShader(shader);

    // Now check for errors
    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    std::cout << "Result: " << result << "\n";
	if (result == GL_FALSE) {
        // Print the error message
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)(alloca(length * (sizeof(char))));
		glGetShaderInfoLog(shader, length, &length, message);
		std::cout << "Failed to compile shader--\n";
		std::cout << message << "\n";
		glDeleteShader(shader);
		std::exit(1);

	}

    return shader;

}

// Debugging function provided by opengl documentation
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    printf("ERROR: %s\n", message);
    // exit(1);
}

int main(int argc, char **argv) {

	// opengl stuff here
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(width, height, "Window", NULL, NULL);
	if (window == NULL) {
		std::cout << "failed to create\n";
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initalize GLAD\n");
		return -1;
	}

    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallBack);
    glfwSetKeyCallback(window, key_callback);

	glViewport(0, 0, width, height);

    // Allow for debuging
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);


    stbi_set_flip_vertically_on_load(true);  

    // Load the cube map
    uint32_t cubeMap;
    glGenTextures(1, &cubeMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

    int width__, height__, channels__;
    uint8_t *face;
   
    std::cout << "EHRE\n";

    /*
        This is how the cube map should be, below I change it to only use the up texture because it think it looks much better
        as the sky only and not including the ground. So I left the proper implemntation here and have mine below it(not commented)
    */

    // face = stbi_load("Assets/Textures/penguins/indigo_lf.jpg", &width__, &height__,&channels__, 3);
    // glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width__, height__,0,GL_RGB,GL_UNSIGNED_BYTE,face);

    // face = stbi_load("Assets/Textures/penguins/indigo_rt.jpg", &width__, &height__,&channels__, 3);
    // glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width__, height__,0,GL_RGB,GL_UNSIGNED_BYTE, face);

    // face = stbi_load("Assets/Textures/penguins/indigo_ft.jpg", &width__, &height__,&channels__, 3);
    // glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width__, height__,0,GL_RGB,GL_UNSIGNED_BYTE, face);

    // face = stbi_load("Assets/Textures/penguins/indigo_bk.jpg", &width__, &height__,&channels__, 3);
    // glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width__, height__,0,GL_RGB,GL_UNSIGNED_BYTE, face);

    // face = stbi_load("Assets/Textures/penguins/indigo_up.jpg", &width__, &height__,&channels__, 3);
    // glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width__, height__,0,GL_RGB,GL_UNSIGNED_BYTE, face);

    // face = stbi_load("Assets/Textures/penguins/indigo_dn.jpg", &width__, &height__,&channels__, 3);
    // glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width__, height__,0,GL_RGB,GL_UNSIGNED_BYTE, face);


    face = stbi_load("Assets/Textures/penguins/indigo_up.jpg", &width__, &height__,&channels__, 3);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width__, height__,0,GL_RGB,GL_UNSIGNED_BYTE,face);

    face = stbi_load("Assets/Textures/penguins/indigo_up.jpg", &width__, &height__,&channels__, 3);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width__, height__,0,GL_RGB,GL_UNSIGNED_BYTE, face);

    face = stbi_load("Assets/Textures/penguins/indigo_up.jpg", &width__, &height__,&channels__, 3);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width__, height__,0,GL_RGB,GL_UNSIGNED_BYTE, face);

    face = stbi_load("Assets/Textures/penguins/indigo_up.jpg", &width__, &height__,&channels__, 3);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width__, height__,0,GL_RGB,GL_UNSIGNED_BYTE, face);

    face = stbi_load("Assets/Textures/penguins/indigo_up.jpg", &width__, &height__,&channels__, 3);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width__, height__,0,GL_RGB,GL_UNSIGNED_BYTE, face);

    face = stbi_load("Assets/Textures/penguins/indigo_up.jpg", &width__, &height__,&channels__, 3);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width__, height__,0,GL_RGB,GL_UNSIGNED_BYTE, face);


    // Other texture stuff to make the cubemap work
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 



    // Load our scene from the json file
    loadScene("Assets/Scenes/scene1.json");



    // Load the texture provided
    int width_, height_, channels_;
    uint8_t *imageData = stbi_load(fileName.c_str(),&width_, &height_, &channels_, 3);

    // Read our vertex and fragment shader code files
    char *vertex = readFile("src/vertex.shader");
    char *fragment = readFile("src/fragment.shader");

    // Create our shaders
    uint32_t vertexShader = genShader(&vertex, GL_VERTEX_SHADER);
    uint32_t fragmentShader = genShader(&fragment, GL_FRAGMENT_SHADER);


    // Our opengl Program
    uint32_t program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glUseProgram(program);

    // Create our Vertex Array Object
    uint32_t VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // For later, allow usage of 0 and 1 locations in shaders
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Depth test enabled
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    
    // Create our texture for the regular billboard
    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_,height_,0,GL_RGB,GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Use the coordinates provided in std::vector<...> billboard for these vertices
    glm::vec3 upperLeft = billboard.at(0);
    glm::vec3 lowerLeft = billboard.at(1);
    glm::vec3 upperRight = billboard.at(2);
    glm::vec3 lowerRight = upperRight - upperLeft + lowerLeft;

    // Specify vertex and texture coord
    std::vector<glm::vec3> bb;
    bb.push_back(lowerLeft);
    bb.push_back(glm::vec3(0,0,0));
    bb.push_back(lowerRight);
    bb.push_back(glm::vec3(1,0,0));
    bb.push_back(upperLeft);
    bb.push_back(glm::vec3(0,1,0));
    bb.push_back(upperRight);
    bb.push_back(glm::vec3(1,1,0));
    bb.push_back(upperLeft);
    bb.push_back(glm::vec3(0,1,0));
    bb.push_back(lowerRight);
    bb.push_back(glm::vec3(1,0,0));


    uint32_t billboard1;
    glGenBuffers(1, & billboard1);
    glBindBuffer(GL_ARRAY_BUFFER, billboard1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * bb.size(), &bb[0], GL_STREAM_DRAW);




    /*
        Create our MVP matrices. M4.js has perspective and lookat so I'll use these from my library
    */
    glm::mat4 proj_ = glm::perspective(glm::radians(fov_angle), (float)width/height, near,far);
    glm::mat4 view_ = glm::lookAt(eye, center, glm::vec3(0,1,0));

    // Only model is for the object
    glm::mat4 modelModel;
    glm::mat4 trans(1);

    // Create the matrix ourself, which is given by the columns
    trans[0] = glm::vec4(1, 0, 0, 0);
    trans[1] = glm::vec4(0, 1, 0, 0);
    trans[2] = glm::vec4(0, 0, 1, 0);
    trans[3] = glm::vec4(position.x, position.y, position.z, 1);

    modelModel = trans;


    /*
        Get the location of several Opengl uniforms
    */
    uint32_t proj = glGetUniformLocation(program, "projection");
    uint32_t view = glGetUniformLocation(program, "view");
    uint32_t model = glGetUniformLocation(program, "model");

    uint32_t color = glGetUniformLocation(program, "Color");
    uint32_t sunLoc = glGetUniformLocation(program, "SunLocation");
    uint32_t isModel = glGetUniformLocation(program, "isModel");


    /*
        Send the data over
    */
    glUniformMatrix4fv(proj, 1, false, glm::value_ptr(proj_));
    glUniformMatrix4fv(view, 1, false, glm::value_ptr(view_));
    glUniformMatrix4fv(model, 1, false, glm::value_ptr(modelModel));

    glUniform3f(color, DefaultColor.x, DefaultColor.y, DefaultColor.z);
    glUniform3f(sunLoc, SunLocation.x, SunLocation.y, SunLocation.z);
    glUniform1i(isModel, 1);

    uint32_t timeLoc = glGetUniformLocation(program, "time");


    // Just a few metrics for performance
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    // Time variable for the shader
    float time = 0.0f;

    while(!glfwWindowShouldClose(window) && !close){

        begin = std::chrono::steady_clock::now();

        // Zero slot is for the image
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Set the uniform location for the cubemap to not collide with the texture itself. We will use slot 8 for the cubemap
        glUniform1i(glGetUniformLocation(program, "cubeMap"), 8);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);



        

        // Calculate view again
        view_ = glm::lookAt(eye, center, glm::vec3(0,1,0));

        // Adjust rr to be in the correct range
        if(rr < 0){
            rr = 0;
        }
        if(rr > 1){
            rr = 1;
        }

        // Send time, amplitude, wavelength, view direction and rr to the shaders
        glUniform1f(timeLoc, time);
        glUniform1f(glGetUniformLocation(program, "amplitude"),amplitude);
        glUniform1f(glGetUniformLocation(program, "wavelength"),wavelength);
        glUniform3fv(glGetUniformLocation(program, "viewDirection"),1,glm::value_ptr(glm::normalize(center-eye)));
        glUniform1f(glGetUniformLocation(program, "rr"), rr);


        // Log the wavelength and amplitude so we can see what it is
        printf("Amp: %f, Wave: %f\n", amplitude, wavelength);



        /*
            Now bind our screen framebuffer so we can render the actual scene
        */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Clear and set background
        glClearColor(.529,.807,.921,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(0,0,width, height);

        // Use the origional view matricies
        glUniformMatrix4fv(view, 1, false, glm::value_ptr(view_));
        glUniformMatrix4fv(proj, 1, false, glm::value_ptr(proj_));


        //Draw the billboard
        glUniform1i(isModel, 0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindBuffer(GL_ARRAY_BUFFER, billboard1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Recompute our modelmatrix and send over
        modelModel[3] = glm::vec4(position.x, position.y, position.z, 1);
        glUniformMatrix4fv(model, 1, false, glm::value_ptr(modelModel));


        // Swap the image buffers, get movement data and upate camera
        glfwSwapBuffers(window);
        glfwPollEvents();
        move();

        end = std::chrono::steady_clock::now();


        // get delta time and frame data
	    milis = (end - begin).count() / 1000000.0;
	    std::cout << "Time difference = " << milis << "[ms]" << " FPS: " << 1000.0 / milis << "\n";

        time += milis/1000;

        // If not pressed, set time back to 0
        if(dPressed == false){
            time = 0;
        }

        // Log any errors
        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR){
            std::cout << "ERROR!\n";
            std::cout << err << "\n";
            // exit(1);
        }
    }

}