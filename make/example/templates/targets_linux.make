bin/$(NAME): $(OBJ) $(OBJ_EXTRA)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

res/globox:
	make/scripts/globox_get.sh

res/wayland_headers:
	make/scripts/wayland_get.sh

leak: bin/$(NAME)
	cd bin && valgrind $(VALGRIND) 2> ../valgrind.log $(CMD)
	less valgrind.log

run: bin/$(NAME)
	cd bin && $(CMD)
