#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/*
#define NK_IMPLEMENTATION
#define NK_PRIVATE
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_FONT_BAKING
#include <nuklear.h>
*/

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb/stb_rect_pack.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include <stb/stb_truetype.h>

#include "utils.h"
#include "qiao_math.c"

int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 768;

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

/*
float
NK_TextWidth(nk_handle handle, float height, const char* text, int len) {
    //TODO
    return 100;
}
*/

void
DrawText(float x, float y, char* text, GLuint tex, stbtt_bakedchar* cdata) {
    glBindTexture(GL_TEXTURE_2D, tex);
    glBegin(GL_QUADS);
    while (*text) {
        if (*text >= 32 && (int)*text < 128) {
            stbtt_aligned_quad quad;
            stbtt_GetBakedQuad(cdata, 1024, 1024, *text - 32, &x, &y, &quad, 1);
            glTexCoord2f(quad.s0, quad.t0); glVertex2f(quad.x0, quad.y0);
            glTexCoord2f(quad.s1, quad.t0); glVertex2f(quad.x1, quad.y0);
            glTexCoord2f(quad.s1, quad.t1); glVertex2f(quad.x1, quad.y1);
            glTexCoord2f(quad.s0, quad.t1); glVertex2f(quad.x0, quad.y1);
        }
        ++text;
    }
    glEnd();
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
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(SCREEN_WIDTH,SCREEN_HEIGHT, "dev", NULL, NULL);
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

    /* stb ttf init */
    unsigned char* ttfBuffer = (unsigned char*)malloc(sizeof(unsigned char) * 1<<25);
    unsigned char* tempBitmap = (unsigned char*)malloc(sizeof(unsigned char) * 1024*1024);
    stbtt_bakedchar cdata[96];
    GLuint tex;

    if (running) {
        printf("Loading font...");
        const char* ttfFileName = "JetBrainsMono-Regular.ttf";
        FILE* ttfFile = fopen(ttfFileName, "rb");
        if (ttfFile == NULL) {
            running = false;
            printf("Failed to load %s\n", ttfFileName);
        } else {
            fread(ttfBuffer, 1, 1<<25,ttfFile);
            stbtt_BakeFontBitmap(ttfBuffer, 0, 64.0, tempBitmap, 1024, 1024, 32, 96, cdata);
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 1024, 1024, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tempBitmap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            fclose(ttfFile);
            printf("Successful.\n");
        }
    }

    /*
    struct nk_context nkContext;
    struct nk_user_font nkFont;
    struct nk_font_atlas atlas;
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);
    struct nk_font* font2 = nk_font_atlas_add_from_memory(&atlas, &ttfBuffer, 16, 0, NULL);
    int img_width, img_height = 0;
    const void* img = nk_font_atlas_bake(&atlas, &img_width, &img_height, NK_FONT_ATLAS_RGBA32);
    nk_font_atlas_end(&atlas, nk_handle_id(texture), 0);
    nkFont.userdata.ptr = &ttfBuffer;
    nkFont.height = 10;
    nkFont.width = NK_TextWidth;

    nk_init_default(&nkContext, &nkFont);
    */

    if (running) {
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glfwPollEvents();
    }

    printf("\nInit done, enter main loop\n");
    while (running &&
           glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0) {
        /*
        for (int i = 0; i < vertexCount; ++i) {
            vertexData[i] = Vec4_Transform(x10, vertexData[i]);
            vertexData[i] = Vec4_Rotate(vertexData[i], 'z', Rad(1.0f));
        }

        GLfloat flatVertexData[9];
        Vec4_Flatten(flatVertexData, vertexData, vertexCount);

        glBufferData(GL_ARRAY_BUFFER, sizeof(flatVertexData), flatVertexData, GL_STATIC_DRAW);

        glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
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
        */

        glClearColor(0.45f, 0.45f, 0.75f, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);

        glColor3f(1,0,0);

        const char* text = "This is a simple test!";
        //DrawText(100,150,"This is a simple test!", tex, cdata);

        glBegin(GL_QUADS);
        float x, y = 0;
        while (*text) {
            if (*text >= 32 && (int)*text < 128) {
                stbtt_aligned_quad q;
                stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1);
                glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y0);
                glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y0);
                glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y1);
                glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y1);
            }
            ++text;
        }
        glEnd();

/*
        glBegin(GL_QUADS);
            glTexCoord2f(0,0); glVertex2f(256,200+0);
            glTexCoord2f(1,0); glVertex2f(768,200+0);
            glTexCoord2f(1,1); glVertex2f(768,200+512);
            glTexCoord2f(0,1); glVertex2f(256,200+512);
        glEnd();
*/
        /* nk section
        if (nk_begin(&nkContext, "Show", nk_rect(50,50,220,220), NK_WINDOW_BORDER)) {
            if (nk_button_label(&nkContext, "button")) {

            }
        } 
        nk_end(&nkContext);
        */

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free(ttfBuffer);
    free(tempBitmap);
    glfwTerminate();
    return 0;
}
