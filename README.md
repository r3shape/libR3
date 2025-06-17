# ![r3-logo](link) **r3shape's standard library**
| **A lightweight runtime library.**


**r3std** is designed to be minimal, extensible, and easy to integrate—ideal for custom runtimes, low-level tools, and other systems-level development.

---
## Features

- **Modular architecture** – Include only the APIs you need.
- **No platform dependencies** – Fully self-contained, written in C99.
- **Runtime metadata** – Track module size, version, function calls, and usage stats.

---

## API Overview
| API     | Description                           |
|---------|---------------------------------------|
| `r3mem`    | Memory management (handles alignment + ptr diffs)  |
| `r3arr`    | Array data structures (static, dynamic, linked...) |
| `r3str`    | String handling/manipulation                    |
| `r3log`    | Logging utilities                               |
| `r3math`   | Math utilities (scalars, vectors, matrices)     |
| `r3file`   | File I/O and stream management                  |

| <b>NOTE:</b> Each module is independently initialized and tracked via runtime metadata.

---

## Getting Started

### 1. Choose a Release

| Release       | Contents                                                                 |
|---------------|--------------------------------------------------------------------------|
| `r3std-dev`   | Full development version: headers, and prebuilt `.dll`    |
| `r3std`       | Standard release: prebuilt `.dll`                         |

| <b>NOTE:</b> Both versions are available as `.zip` and `.tar` archives.

---

### 2. Development Integration

Use the `r3std-dev` release if you want access to all headers, and prebuilt binaries:

- Add r3std's `include` directory to your compiler's header search path
- Link against the provided `.dll` build artifact.
- Use `r3std.h` to bootstrap the library and access the full API

---

### 3. Write Some Code
```c
// #include <include/r3std/r3std.h> // include the enire standard or
#include <include/r3std/r3mem.h>    // include the standard API's you need

int main() {
    void* some_memory;

    // allocate 1024 bytes, 8 byte aligned
    if ((some_memory = r3_alloc(1024, 8))) {
        // code away...
    } else { return 1; } // out of memory!

    r3_dealloc(some_memory); // don't froget to free...
    return 0;
}
```

---

## Building From Source

### Option 1: Use your preferred build system or a minimal setup:

```bash
gcc -shared -o r3std.dll src/r3std/*.c
```

### Option 2: The build process can be automated using [`r3make`](https://github.com/r3shape/r3make).

```bash
r3make
```
| <b>NOTE:</b> This command builds the SSDK library

```bash
r3make -be -r -t tests
```
| <b>NOTE:</b> This command builds, and runs the SSDK library tests

---

## r3std Contributors

<a href="https://github.com/r3shape/r3std/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=r3shape/r3std" />
</a>

## License

r3std is released under the MIT License.

