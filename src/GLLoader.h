#ifndef __GL_LOADER
#define __GL_LOADER

static float initLoader()
{
	float ver = 0.0f;

	if (gl3wInit()) {
		fprintf(stderr, "failed to initialize OpenGL\n");
	}

	if (gl3wIsSupported(1, 1)) { ver = 1.1f; }
	if (gl3wIsSupported(1, 2)) { ver = 1.2f; }
	if (gl3wIsSupported(1, 3)) { ver = 1.3f; }
	if (gl3wIsSupported(1, 4)) { ver = 1.4f; }
	if (gl3wIsSupported(1, 5)) { ver = 1.5f; }

	if (gl3wIsSupported(2, 0)) { ver = 2.0f; }
	if (gl3wIsSupported(2, 1)) { ver = 2.1f; }

	if (gl3wIsSupported(3, 0)) { ver = 3.0f; }
	if (gl3wIsSupported(3, 1)) { ver = 3.1f; }
	if (gl3wIsSupported(3, 2)) { ver = 3.2f; }
	if (gl3wIsSupported(3, 3)) { ver = 3.3f; }

	if (gl3wIsSupported(4, 0)) { ver = 4.0f; }
	if (gl3wIsSupported(4, 1)) { ver = 4.1f; }
	if (gl3wIsSupported(4, 2)) { ver = 4.2f; }
	if (gl3wIsSupported(4, 3)) { ver = 4.3f; }
	if (gl3wIsSupported(4, 4)) { ver = 4.4f; }
	if (gl3wIsSupported(4, 5)) { ver = 4.5f; }

	return ver;
}

#endif
