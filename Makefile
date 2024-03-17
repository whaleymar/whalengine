.PHONY: all clean

all:
	cmake . -B build && $(MAKE) -C build -j

clean:
	rm -r build || true

