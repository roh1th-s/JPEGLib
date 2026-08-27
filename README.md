# jpeg-zero

A simple, barebones implementation of the JPEG compression standard for educational purposes :).

**Note**:
- Only baseline (sequential) DCT is supported.
- No support for chroma subsampling.
- Only decoder implemented so far.

# Building

Build the project with:

```sh
make
```

Run it with

```sh
# Creates a decoded .ppm file in the same directory
./build/main image.jpg
```

# Tests

Some basic tests are included, which can be run with:

```sh
make test
```

# References

- https://koushtav.me/jpeg/tutorial/2017/11/25/lets-write-a-simple-jpeg-library-part-1/
- https://www.opennet.ru/docs/formats/jpeg.txt
