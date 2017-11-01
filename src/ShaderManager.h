#ifndef __SHADERMANAGER_
#define __SHADERMANAGER_

#include <SFML/Window.hpp>
#include <stdio.h>

#if _MSC_VER
#define snprintf _snprintf
#endif

class ShaderManager
{
public:
	
	static GLuint shaderFromFile(char const * const * vertShaderPath, char const * const * fragShaderPath, int const vertShaderCount, int const fragShaderCount)
	{
		return readAndBuildProgram(vertShaderPath, fragShaderPath, vertShaderCount, fragShaderCount);
	}
	
	static GLuint shaderFromString(char const * const * vertShaderSource, char const * const * fragShaderSource, int const vertShaderCount, int const fragShaderCount)
	{
		char const path[] = "no path";
		char const * const p = (char const * const) &path;
		return createShaderProgram(vertShaderSource, fragShaderSource, &p, &p, vertShaderCount, fragShaderCount);
	}
	
private:
	static GLuint readAndBuildProgram(char const * const * vertShaderPath, char const * const * fragShaderPath, int const vertShaderCount, int const fragShaderCount)
	{
		#ifndef MAX_SHADERS
		#define MAX_SHADERS 10
		#endif

		if(vertShaderCount > MAX_SHADERS || fragShaderCount > MAX_SHADERS)
		{
			printf("This function can only build shaders with %i files. Exiting.\n", MAX_SHADERS);
			exit(1);
		}

		char ** newVertPath = (char**) malloc(sizeof(char*) * vertShaderCount);
		char ** newFragPath = (char**) malloc(sizeof(char*) * fragShaderCount);
		
		#define MAX_PATH_SIZE 800
		for(int i=0; i<vertShaderCount; i++)
			newVertPath[i] = (char*) malloc(sizeof(char) * MAX_PATH_SIZE);
		for(int i=0; i<fragShaderCount; i++)
			newFragPath[i] = (char*) malloc(sizeof(char) * MAX_PATH_SIZE);
		
		for(int i=0; i<vertShaderCount; i++)
			snprintf(newVertPath[i], MAX_PATH_SIZE, "%s", vertShaderPath[i]);
		for(int i=0; i<fragShaderCount; i++)
			snprintf(newFragPath[i], MAX_PATH_SIZE, "%s", fragShaderPath[i]);
		
		char * vertSource[MAX_SHADERS];
		char * fragSource[MAX_SHADERS];
		
		//read shader source
		for(int i=0; i<vertShaderCount; i++)
			vertSource[i] = readFile( newVertPath[i] );
		for(int i=0; i<fragShaderCount; i++)
			fragSource[i] = readFile( newFragPath[i] );
		
		//create shaders
		GLuint prog = createShaderProgram(vertSource, fragSource, vertShaderPath, fragShaderPath, vertShaderCount, fragShaderCount);
		
		for(int i=0; i<vertShaderCount; i++)
			free( (void*) vertSource[i]);
		for(int i=0; i<fragShaderCount; i++)
			free( (void*) fragSource[i]);

		for(int i=0; i<vertShaderCount; i++)
			free(newVertPath[i]);
		for(int i=0; i<fragShaderCount; i++)
			free(newFragPath[i]);
		
		return prog;
	}
	
	static GLuint createShaderProgram(char const * const * vertSource, char const * const * fragSource, char const * const * vertShaderPath, char const * const * fragShaderPath, int const vertShaderCount, int const fragShaderCount)
	{
#ifndef MAX_SHADERS
#define MAX_SHADERS 10
#endif
		if(vertShaderCount > MAX_SHADERS || fragShaderCount > MAX_SHADERS)
		{
			printf("This function can only build shaders with %i files. Exiting.\n", MAX_SHADERS);
			exit(1);
		}
		
		GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		
		glShaderSource(vertShader, vertShaderCount, (char const **)vertSource, NULL);
		glShaderSource(fragShader, fragShaderCount, (char const **)fragSource, NULL);
		
		glCompileShader(vertShader);
		printShaderInfoLog(vertShader, vertShaderPath, vertShaderCount);
		glCompileShader(fragShader);
		printShaderInfoLog(fragShader, fragShaderPath, fragShaderCount);
		
		GLuint prog = glCreateProgram();
		glAttachShader(prog, vertShader);
		glAttachShader(prog, fragShader);
		
		glLinkProgram(prog);
		printProgramInfoLog(prog, vertShaderPath, fragShaderPath, vertShaderCount, fragShaderCount);
		
		return prog;
	}
	
	static void printShaderInfoLog(GLuint obj, char const * const * shaderPath, int const shaderCount)
	{
		int infologLength = 0;
		int charsWritten  = 0;
		char *infoLog;
		
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
		
		if (infologLength > 1)
		{
			printf("Shader info log for:\n");
			for (size_t i=0; i<shaderCount; i++)
				printf("%s\n", shaderPath[i]);

			infoLog = (char *)malloc(infologLength);
			glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
			printf("%s\n",infoLog);
			free(infoLog);
			
			GLint success;
			glGetShaderiv(obj, GL_COMPILE_STATUS, &success);
			if(success != GL_TRUE)
				exit(4);
		}
		else
		{
		}
	}
	
	static void printProgramInfoLog(GLuint obj, char const * const * vertShaderPath, char const * const * fragShaderPath, int const vertShaderCount, int const fragShaderCount)
	{
		int infologLength = 0;
		int charsWritten  = 0;
		char *infoLog;
		
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
		
		if (infologLength > 1)
		{
			printf("Shader program info log for:\n");
			for (size_t i = 0; i<vertShaderCount; i++)
				printf("%s\n", vertShaderPath[i]);
			for (size_t i = 0; i<fragShaderCount; i++)
				printf("%s\n", fragShaderPath[i]);

			infoLog = (char *)malloc(infologLength);
			glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
			printf("%s\n",infoLog);
			free(infoLog);
			
			GLint success;
			glGetProgramiv(obj, GL_LINK_STATUS, &success);
			if(success != GL_TRUE)
				exit(4);
		}
	}
	
	static FILE* sfopen(const char* name, const char *mode)
	{
		FILE *file = NULL;
		file = fopen(name, mode);
		
		if(file == NULL)
		{
			printf("Unable to open file '%s' in mode '%s'\n", name, mode);
			exit(1);
		}
		
		return file;
	}
	
	static char* readFile(char const * fileName)
	{
		FILE *file;
		
		//open file
		file = sfopen(fileName, "rb");
		
		//get size
		fseek(file, 0, SEEK_END);
		int fileDataLength = ftell(file);
		rewind(file);
		
		//get memory for the data
		char * fileData = new char[fileDataLength+1];
		fread(fileData, 1, fileDataLength, file);
		
		fclose(file);
		fileData[fileDataLength] = '\0';
		
		return fileData;
	}

};

#endif
