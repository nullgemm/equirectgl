bin/$(NAME): $(OBJ) $(OBJ_EXTRA)
	mkdir -p $(@D)
	$(CC) -Febin/$(NAME).exe $^ -link -ENTRY:mainCRTStartup $(LDFLAGS) $(LDLIBS)

bin/eglproxy.dll: res/eglproxy
	mkdir -p $(@D)
	cp res/eglproxy/lib/msvc/eglproxy.dll $@

res/eglproxy:
	make/scripts/eglproxy_get.sh

res/egl_headers:
	make/scripts/egl_get.sh

res/globox:
	make/scripts/globox_get.sh

run: bin/$(NAME)
	cd bin && $(CMD)

leak: bin/$(NAME).exe
	cd bin && drmemory.exe $(DRMEMORY) 2> ../drmemory.log $(CMD)
	less drmemory.log

.SUFFIXES: .c .obj
.c.obj:
	$(CC) $(CFLAGS) -Fo$@ -c $<
