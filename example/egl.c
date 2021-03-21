#include "globox.h"
#include "globox_private_getters.h"

#include "willis.h"
#include "spng.h"

#include <stddef.h>
#include <stdio.h>

#if defined(GLOBOX_PLATFORM_WINDOWS)
#include <GL/gl.h>
#define GL_GLES_PROTOTYPES 0
#endif

#include <GLES2/gl2.h>

#define VERTEX_ATTR_POSITION 0
#define M_PI 3.141592653589793238462643383279502884197169f

GLfloat fov_loc;
GLfloat view_width_loc;
GLfloat view_height_loc;
GLfloat view_angle_lat_loc;
GLfloat view_angle_lon_loc;

float fov = 90.0f;
float angle_lat = 0.0f;
float angle_lon = M_PI / 2.0f;

#if defined(GLOBOX_PLATFORM_WINDOWS)
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

// opengl32.dll only supports OpenGL 1
// so we have to load these functions
void wgl_load()
{
	glAttachShader =
		(PFNGLATTACHSHADERPROC)
			wglGetProcAddress("glAttachShader");
	glCompileShader =
		(PFNGLCOMPILESHADERPROC)
			wglGetProcAddress("glCompileShader");
	glCreateProgram =
		(PFNGLCREATEPROGRAMPROC)
			wglGetProcAddress("glCreateProgram");
	glCreateShader =
		(PFNGLCREATESHADERPROC)
			wglGetProcAddress("glCreateShader");
	glDeleteShader =
		(PFNGLDELETESHADERPROC)
			wglGetProcAddress("glDeleteShader");
	glEnableVertexAttribArray =
		(PFNGLENABLEVERTEXATTRIBARRAYPROC)
			wglGetProcAddress("glEnableVertexAttribArray");
	glLinkProgram =
		(PFNGLLINKPROGRAMPROC)
			wglGetProcAddress("glLinkProgram");
	glShaderSource =
		(PFNGLSHADERSOURCEPROC)
			wglGetProcAddress("glShaderSource");
	glUseProgram =
		(PFNGLUSEPROGRAMPROC)
			wglGetProcAddress("glUseProgram");
	glVertexAttribPointer =
		(PFNGLVERTEXATTRIBPOINTERPROC)
			wglGetProcAddress("glVertexAttribPointer");
}
#endif

void render(struct globox* globox)
{
	globox_platform_events_handle(
		globox);

	if (globox_error_catch(globox))
	{
		return;
	}

	if (true)
	{
		int32_t width = globox_get_width(globox);
		int32_t height = globox_get_height(globox);
		GLint viewport_rect[4];

		// we can make OpenGL 1 calls without any loader
		glGetIntegerv(GL_VIEWPORT, viewport_rect);

		if ((viewport_rect[2] != width) || (viewport_rect[3] != height))
		{
			glViewport(0, 0, width, height);
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLfloat vertices[] =
		{
			-1.0f, +1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			+1.0f, -1.0f, 1.0f,
			+1.0f, +1.0f, 1.0f,
		};

		glUniform1f(view_width_loc, width);
		glUniform1f(view_height_loc, height);
		glUniform1f(fov_loc, fov / 180.0f * M_PI);
		glEnableVertexAttribArray(VERTEX_ATTR_POSITION);

		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			vertices);

		glDrawArrays(
			GL_TRIANGLE_FAN,
			0,
			4);

		globox_context_egl_copy(
			globox,
			0,
			0,
			width,
			height);
	}
}

void event(
	struct willis* willis,
	enum willis_event_code event_code,
	enum willis_event_state event_state,
	void* data)
{
	if ((event_code == WILLIS_KEY_Q)
	&& (event_state == WILLIS_STATE_PRESS))
	{
		willis_mouse_grab(willis);
	}

	if ((event_code == WILLIS_KEY_W)
	&& (event_state == WILLIS_STATE_PRESS))
	{
		willis_mouse_ungrab(willis);
	}

	if (event_code == WILLIS_MOUSE_WHEEL_DOWN)
	{
		fov += 3.0f;

		if (fov > 360.0f)
		{
			fov = 360.0f;
		}
	}

	if (event_code == WILLIS_MOUSE_WHEEL_UP)
	{
		fov -= 3.0f;

		if (fov < 0.0f)
		{
			fov = 0.0f;
		}
	}

	if ((event_code == WILLIS_MOUSE_MOTION) && (willis_get_mouse_grab(willis) == true))
	{
		angle_lat += willis_get_diff_x(willis) / 1000.0f / 4294967296.0f;
		angle_lon += willis_get_diff_y(willis) / 1000.0f / 4294967296.0f;

		if (angle_lon > M_PI)
		{
			angle_lon = M_PI;
		}
		else if (angle_lon < 0)
		{
			angle_lon = 0;
		}

		glUniform1f(view_angle_lat_loc, angle_lat);
		glUniform1f(view_angle_lon_loc, angle_lon);
	}
}

int main(void)
{
	struct globox globox = {0};
	struct willis willis = {0};
	void* willis_backend_link = NULL;

	// load png
	FILE* png = fopen("res/img/perseverance_panorama.png", "rb");

	if (png == NULL)
	{
		return 1;
	}

	spng_ctx* png_ctx = spng_ctx_new(0);

	if (png_ctx == NULL)
	{
		return 1;
	}

	spng_set_crc_action(png_ctx, SPNG_CRC_USE, SPNG_CRC_USE);
	spng_set_png_file(png_ctx, png);

	int ok;
	size_t png_size;
	struct spng_ihdr ihdr;

    ok = spng_get_ihdr(png_ctx, &ihdr);

	if (ok != 0)
	{
		spng_ctx_free(png_ctx);
		return 1;
	}

	ok = spng_decoded_image_size(png_ctx, SPNG_FMT_RGBA8, &png_size);

	if (ok != 0)
	{
		spng_ctx_free(png_ctx);
		return 1;
	}

	unsigned char* png_out = malloc(png_size);

	if (png_out == NULL)
	{
		spng_ctx_free(png_ctx);
		return 1;
	}

	ok = spng_decode_image(png_ctx, png_out, png_size, SPNG_FMT_RGBA8, 0);

	if (ok != 0)
	{
		spng_ctx_free(png_ctx);
		free(png_out);
		return 1;
	}

	// start initializing globox
	globox_open(
		&globox,
		0,
		0,
		500,
		500,
		"globox",
		GLOBOX_STATE_REGULAR,
		willis_handle_events,
		&willis);

	if (globox_error_catch(&globox))
	{
		return 1;
	}

	globox_platform_init(&globox, true, false, true);

	if (globox_error_catch(&globox))
	{
		globox_close(&globox);
		return 1;
	}

	// use OpenGL 2 or glES 2
	globox_context_egl_init(&globox, 2, 0);

	if (globox_error_catch(&globox))
	{
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	globox_platform_create_window(&globox);

	if (globox_error_catch(&globox))
	{
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	// finish initializing globox
	globox_context_egl_create(&globox);

	if (globox_error_catch(&globox))
	{
		globox_context_egl_free(&globox);
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	globox_platform_hooks(&globox);

	if (globox_error_catch(&globox))
	{
		globox_context_egl_free(&globox);
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

#if defined(GLOBOX_PLATFORM_WINDOWS)
	// load OpenGL functions
	wgl_load();
#endif

	glEnable (GL_TEXTURE_2D);

	GLuint tex = 0;
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		ihdr.width,
		ihdr.height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		png_out);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// prepare OpenGL or glES
	const char* vertex_shader_src =
		"#version 130\n"
		"attribute vec4 vPosition;\n"
		"void main()\n"
		"{\n"
		"\tgl_Position = vPosition;\n"
		"}\n";

	const char* fragment_shader_src =
		"#version 130\n"
		"#define M_PI 3.141592653589793238462643383279502884197169f\n"
		"precision mediump float;\n"
		"uniform sampler2D tex;\n"
		"uniform float tex_width;\n"
		"uniform float tex_height;\n"
		"uniform float fov;\n"
		"uniform float view_width;\n"
		"uniform float view_height;\n"
		"uniform float view_angle_lat;\n"
		"uniform float view_angle_lon;\n"
		"void main()\n"
		"{\n"
		"\tfloat tex_radius = tex_width / (2.0f * M_PI);\n"
		"\tfloat view_dist = (view_width / 2.0f) / tan(fov / 2.0f);\n"
		"\tfloat vert_fov = 2.0f * atan(view_height / 2.0f, view_dist);\n"
		"\tfloat view_lat = ((view_width / 2.0f) - gl_FragCoord.x) / view_width * fov;\n"
		"\tfloat view_lon = view_angle_lon + ((view_height / 2.0f) - gl_FragCoord.y) / view_height * vert_fov;\n"
		"\tfloat x = cos(view_lon - (M_PI / 2));\n"
		"\tfloat z = sin((M_PI / 2) - view_lon);\n"
		"\tfloat y = sin((M_PI * 2) - view_lat);\n"
		"\tfloat r = sqrt(x*x + y*y + z*z);"
		"\tgl_FragColor = texture2D(tex, vec2(tex_radius * (atan(y, x) + view_angle_lat) / tex_width, tex_radius * acos(z / r) / tex_height));\n"
		"}\n";

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_src, 0);
	glCompileShader(vertex_shader);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_src, 0);
	glCompileShader(fragment_shader);

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glLinkProgram(shader_program);
	glUseProgram(shader_program);

	// bind texture
	GLint tex_loc = glGetUniformLocation(shader_program, "tex");
	GLfloat tex_width_loc = glGetUniformLocation(shader_program, "tex_width");
	GLfloat tex_height_loc = glGetUniformLocation(shader_program, "tex_height");

	view_width_loc = glGetUniformLocation(shader_program, "view_width");
	view_height_loc = glGetUniformLocation(shader_program, "view_height");
	fov_loc = glGetUniformLocation(shader_program, "fov");

	view_angle_lat_loc = glGetUniformLocation(shader_program, "view_angle_lat");
	view_angle_lon_loc = glGetUniformLocation(shader_program, "view_angle_lon");

	glUniform1i(tex_loc, 0);
	glUniform1f(tex_width_loc, ihdr.width);
	glUniform1f(tex_height_loc, ihdr.height);
	glUniform1f(view_angle_lat_loc, angle_lat);
	glUniform1f(view_angle_lon_loc, angle_lon);

	// continue initializing globox
	globox_platform_commit(&globox);

	render(&globox);

	// willis
#if defined(WILLIS_X11)
	struct willis_data_x11 willis_data =
	{
		.x11_conn =
			globox_get_x11_conn(&globox),
		.x11_root =
			globox_get_x11_root_win(&globox),
		.x11_window =
			globox_get_x11_win(&globox),
	};

	willis_backend_link = &willis_data;
#elif defined(WILLIS_WAYLAND)
	struct willis_data_wayland willis_data =
	{
		.wl_surface =
			globox_get_wayland_surface(&globox),
		.wl_relative_pointer =
			globox_get_wayland_pointer_manager(&globox),
		.wl_pointer_constraints =
			globox_get_wayland_pointer_constraints(&globox),
	};

	willis_backend_link = &willis_data;
#elif defined(WILLIS_WINDOWS)
	willis_backend_link = NULL;
#elif defined(WILLIS_MACOS)
	willis_backend_link = NULL;
#endif

	willis_init(
		&willis,
		willis_backend_link,
		true,
		event,
		NULL);

	// main loop
	while (globox_get_closed(&globox) == false)
	{
		globox_platform_prepoll(&globox);

		if (globox_error_catch(&globox))
		{
			globox_context_egl_free(&globox);
			globox_platform_free(&globox);
			globox_close(&globox);
			return 1;
		}

		globox_platform_events_wait(&globox);

		if (globox_error_catch(&globox))
		{
			globox_context_egl_free(&globox);
			globox_platform_free(&globox);
			globox_close(&globox);
			return 1;
		}

		render(&globox);

		if (globox_error_catch(&globox))
		{
			globox_context_egl_free(&globox);
			globox_platform_free(&globox);
			globox_close(&globox);
			return 1;
		}
	}

	spng_ctx_free(png_ctx);
	free(png_out);
	willis_free(&willis);
	globox_context_egl_free(&globox);
	globox_platform_free(&globox);
	globox_close(&globox);

	return 0;
}
