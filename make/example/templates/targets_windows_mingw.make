bin/$(NAME): $(OBJ) $(OBJ_EXTRA)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

bin/eglproxy.dll: res/eglproxy
	mkdir -p $(@D)
	cp res/eglproxy/lib/mingw/eglproxy.dll $@

res/eglproxy:
	make/scripts/eglproxy_get.sh

res/egl_headers:
	make/scripts/egl_get.sh

res/globox:
	make/scripts/globox_get.sh

run: bin/$(NAME)
	cd bin && $(CMD)
