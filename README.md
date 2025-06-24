# ![r3kt-logo](link) **r3shape's kit of tools**

**r3kt** is designed to be minimal, and extensible, making writing complex programs simpler by reducing bugs and boilerplate, helping you ship faster.

---
## Features

- **Modular architecture** – Include only the APIs you need.
- **No platform dependencies** – Fully self-contained, written in C99.
- **Runtime metadata** – Track module size, version, function calls, and usage stats.

---

## API Overview
| API     | Description                           |
|---------|---------------------------------------|
| `r3ktmem`    | Memory management |
| `r3ktarr`    | Array data structures |
| `r3ktstr`    | String handling/manipulation |
| `r3ktlog`    | Logging utilities |
| `r3ktmath`   | Math utilities (GL friendly) |
| `r3ktfile`   | File handling/manipulation |

---

## Getting Started

### 1. Choose a Release

| Release       | Contents                                                                 |
|---------------|--------------------------------------------------------------------------|
| `r3kt-dev`    | Full development version: headers, and prebuilt `.dll` |
| `r3kt-rt`     | Standard featureset + r3kt runtime: prebuilt `.dll` |
| `r3kt`        | Standard release: prebuilt `.dll` |

| <b>NOTE:</b> Both versions are available as `.zip` and `.tar` archives.

---

### 2. Development Integration

Use the `r3kt-dev` release if you want access to all headers, and prebuilt binaries:

- Add r3kt's `include` directory to your compiler's header search path
- Link against the provided `.dll` build artifact.
- Use `r3kt.h` to bootstrap the library and access the full API

---

### 3. Write Some Code
```c
#include <include/r3kt/r3ktmem.h>    // include the API's you need

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
gcc -shared -o r3kt.dll src/r3kt/*.c
```

### Option 2: The build process can be automated using [`r3make`](https://github.com/r3shape/r3make).

```bash
r3make -nf
```
| <b>NOTE:</b> This command builds the r3kt library

```bash
r3make -nf -t r3ktrt
```
| <b>NOTE:</b> This command builds, and runs the r3kt library with runtime features

```bash
r3make -nf -be -r -t tests
```
| <b>NOTE:</b> This command builds, and runs the r3kt library tests

---

## r3kt Contributors

<a href="https://github.com/r3shape/r3kt/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=r3shape/r3kt" />
</a>

## License

r3kt is released under the MIT License.

