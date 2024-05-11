#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//#define NK_IMPLEMENTATION
//#include <nuklear.h>

//#define STB_TRUETYPE_IMPLEMENTATION
//#include <stb/stb_truetype.h>

#include "utils.h"
#include "qiao_math.c"

GLuint LoadShaders(const char * vertexShaderPath,const char * fragmentShaderPath) {
    /* Create the shaders */
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    FILE* vertexShaderFile = fopen(vertexShaderPath, "r");

    if (vertexShaderFile == NULL) {
        printf("Couldn't Open File: %s\n", vertexShaderPath);
        return -1;
    }

    fseek(vertexShaderFile, 0, SEEK_END);
    int fileSize = ftell(vertexShaderFile);
    rewind(vertexShaderFile);

    char* vertexShaderCode = (char*)malloc((fileSize + 1) * sizeof(char));
    if (vertexShaderCode == NULL) {
        printf("Shader Code Memory Allocation Failed\n");
        fclose(vertexShaderFile);
        return -1;
    }

    if (fread(vertexShaderCode, sizeof(char), fileSize, vertexShaderFile) != fileSize) {
        printf("Error Reading File %s \n", vertexShaderPath);
        fclose(vertexShaderFile);
        free(vertexShaderCode);
        return -1;
    }

    vertexShaderCode[fileSize] = '\0';
    fclose(vertexShaderFile);

    GLint result = GL_FALSE;
    int infoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader: %s\n", vertexShaderPath);
    const GLchar* vertexShaderPointer = vertexShaderCode;
    glShaderSource(vertexShaderID, 1, &vertexShaderPointer , &fileSize);
    glCompileShader(vertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0){
        char* errorMessage = (char*)malloc((infoLogLength + 1) * sizeof(char));
        glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, errorMessage);
        printf("%s\n", errorMessage);
        free(errorMessage);
    }

    // Read the Vertex Shader code from the file
    FILE* fragmentShaderFile = fopen(fragmentShaderPath, "r");

    if (fragmentShaderFile == NULL) {
        printf("Couldn't Open File: %s\n", fragmentShaderPath);
        return -1;
    }

    fseek(fragmentShaderFile, 0, SEEK_END);
    fileSize = ftell(fragmentShaderFile);
    rewind(fragmentShaderFile);

    char* fragmentShaderCode = (char*)malloc((fileSize + 1) * sizeof(char));
    if (fragmentShaderCode == NULL) {
        printf("Shader Code Memory Allocation Failed\n");
        fclose(fragmentShaderFile);
        return -1;
    }

    if (fread(fragmentShaderCode, sizeof(char), fileSize, fragmentShaderFile) != fileSize) {
        printf("Error Reading File %s \n", fragmentShaderPath);
        fclose(fragmentShaderFile);
        free(fragmentShaderCode);
        return -1;
    }

    fragmentShaderCode[fileSize] = '\0';
    fclose(fragmentShaderFile);

    result = GL_FALSE;

    // Compile Fragment Shader
    printf("Compiling shader: %s\n", fragmentShaderPath);
    const GLchar* fragmentShaderPointer = fragmentShaderCode;
    glShaderSource(fragmentShaderID, 1, &fragmentShaderPointer , NULL);
    glCompileShader(fragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0){
        char* errorMessage = (char*)malloc((infoLogLength + 1) * sizeof(char));
        glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, errorMessage);
        printf("%s\n", errorMessage);
        free(errorMessage);
    }

    // Link the program
    printf("Linking Shader Program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, vertexShaderID);
    glAttachShader(ProgramID, fragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 0 ){
        char* errorMessage = (char*)malloc((infoLogLength + 1) * sizeof(char));
        glGetProgramInfoLog(fragmentShaderID, infoLogLength, NULL, errorMessage);
        printf("%s\n", errorMessage);
        free(errorMessage);
    }

    glDetachShader(ProgramID, vertexShaderID);
    glDetachShader(ProgramID, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    if (vertexShaderCode != NULL) free(vertexShaderCode);
    if (fragmentShaderCode != NULL) free(fragmentShaderCode);

    return ProgramID;
}

void
error_callback(int error, const char* description) {
    printf("Error %d: %s\n", error, description);
}

int
main() {
    bool running = true;
    glewExperimental = true;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        running = false;
    } else {
        printf("GLFW initialized.\n");
    }

    /* Create Window */
    GLFWwindow* window = NULL;
    if (running) {
        printf("Creating Window...");
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(1080,720, "dev", NULL, NULL);
        if (window == NULL) {
            printf("Failed to open GLFW window\n");
            glfwTerminate();
            running = false;
        } else {
            printf("Successful.\n");
        }
    }

    /* Create GL Context */
    if (running) {
        printf("Create GL Context...");
        glfwMakeContextCurrent(window);
        glewExperimental = true;
        if (glewInit() != GLEW_OK) {
            printf("Failed to initialize GLEW\n");
            running = false;
        } else {
            printf("Successful.\n");
        }
    }

    GLuint vertexArrayID, vertexBuffer, programID = 0;
    const int vertexCount = 3;
    Vec4 vertexData[vertexCount];
    vertexData[0] = Vec4_Init_xyz(-1.0f, -1.0f, 0.0f);
    vertexData[1] = Vec4_Init_xyz(1.0f, -1.0f, 0.0f);
    vertexData[2] = Vec4_Init_xyz(0.0f, 1.0f, 0.0f);

    Matrix4x4 x10 = Matrix4x4_Init(
        (float[16]) {
        1, 0, 0, 0.001f,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1});

    if (running) {
        printf("Generating shader programs...\n");
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

        const char* vertexShaderPath = "vertex.glsl";
        const char* fragmentShaderPath = "fragment.glsl";
        programID = LoadShaders(vertexShaderPath, fragmentShaderPath);

        if (programID == -1) {
            printf("Failed.\n");
            running = false;
        } else {
            printf("Successful.\n");
        }
    }

    /*
    unsigned char* ttfBuffer = (unsigned char*)malloc(sizeof(unsigned char) * 1<<25);
    stbtt_fontinfo font;
    //struct nk_user_font nkFont;
    if (running) {
        printf("Loading font...");
        const char* ttfFileName = "JetBrainsMono-Regular.ttf";
        FILE* ttfFile = fopen(ttfFileName, "rb");
        if (ttfFile == NULL) {
            running = false;
            printf("Failed to load %s\n", ttfFileName);
        } else {
            fread(ttfBuffer, 1, 1<<25,ttfFile);
            stbtt_InitFont(&font, ttfBuffer, stbtt_GetFontOffsetForIndex(ttfBuffer, 0));
            fclose(ttfFile);
            printf("Successful.\n");
        }
    }
    */

    /*
    struct nk_context nkContext;
    struct nk_user_font nkFont;
    nk_init_fixed(&nkContext, calloc(1,4096), 4096, &nkFont);
*/

    if (running) {
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
    }

    printf("\nInit done, enter main loop\n");
    while (running && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        for (int i = 0; i < vertexCount; ++i) {
            vertexData[i] = Vec4_Transform(x10, vertexData[i]);
            vertexData[i] = Vec4_Rotate(vertexData[i], 'z', Rad(1.0f));
        }

        GLfloat flatVertexData[9];
        Vec4_Flatten(flatVertexData, vertexData, vertexCount);

        glBufferData(GL_ARRAY_BUFFER, sizeof(flatVertexData), flatVertexData, GL_STATIC_DRAW);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. Matches the layout in the shader.
            vertexCount,        // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        /* nk section
        if (nk_begin(&nkContext, "Show", nk_rect(50,50,220,220), NK_WINDOW_BORDER)) {

        } 
        nk_end(&nkContext);
        */

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //free(ttfBuffer);
    glfwTerminate();
    return 0;
}
