#ifndef __GLHELPER
#define __GLHELPER

static void checkGLError(char const * ident = "")
{
	GLenum errCode;
	
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		fprintf(stderr, "OpenGL Error (%s): ", ident);
		switch (errCode)
		{
		case GL_INVALID_ENUM:
			fprintf(stderr, "GL_INVALID_ENUM\n");
			break;
		case GL_INVALID_VALUE:
			fprintf(stderr, "GL_INVALID_VALUE\n");
			break;
		case GL_INVALID_OPERATION:
			fprintf(stderr, "GL_INVALID_OPERATION\n");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			fprintf(stderr, "GL_INVALID_FRAMEBUFFER_OPERATION\n");
			break;
		case GL_OUT_OF_MEMORY:
			fprintf(stderr, "GL_OUT_OF_MEMORY\n");
			break;
		}
		exit(4);
	}
}

static void checkGLFrameBuffer(GLenum target, char const * ident = "")
{
	GLenum errCode;
	
	if ((errCode = glCheckFramebufferStatus(target)) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "FrameBuffer Status (%s): ", ident);
		switch (errCode)
		{
			case GL_FRAMEBUFFER_UNDEFINED:
			fprintf(stderr, "GL_FRAMEBUFFER_UNDEFINED\n");
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n");
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n");
			break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
			fprintf(stderr, "GL_FRAMEBUFFER_UNSUPPORTED\n");
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n");
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS\n");
			break;
			case 0:
			fprintf(stderr, "Error occurred\n");
			break;
		}
		exit(4);
	}
}

#endif