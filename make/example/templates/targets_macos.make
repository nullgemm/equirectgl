bin/$(NAME).app: bin/$(NAME)
	mv bin/$(NAME) $@

bin/$(NAME): $(OBJ) $(OBJ_EXTRA)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

bin/libEGL.dylib: res/angle/libs
	cp res/angle/libs/*.dylib bin/

res/angle/libs:
	mkdir -p bin
	make/scripts/angle_dev_get.sh

res/globox:
	make/scripts/globox_get.sh

run: bin/$(NAME)
	cd bin && $(CMD)
