# ![r3shape-labs](https://github.com/user-attachments/assets/ac634f13-e084-4387-aded-4679eb048cac)  <br> **r3shape's kit of tools**

**r3kit** is designed to be minimal, and extensible, making writing complex programs simpler by reducing bugs and boilerplate, helping you ship faster.

---
## Features

- **Modular architecture** – Include only the APIs you need.
- **No platform dependencies** – Fully self-contained, written in C99.
- **Simple Integration** - Any api can serve as a drop-in along side the `r3kit.mem.alloc` API.

---

## API Overview
| API     | Description                           |
|---------|---------------------------------------|
| `mem`     | Memory management |
| `ds`      | Data Structures   |
| `io`      | Input/Output      |
| `math`    | Math utilities (GL friendly) |

---

## Getting Started

### 1. Choose a Release

| Release       | Contents                                                                 |
|---------------|--------------------------------------------------------------------------|
| `r3kit-dev`    | Full development version: headers, and prebuilt `.dll` |
| `r3kit`        | Standard release: prebuilt `.dll` |

| <b>NOTE:</b> Both versions are available as `.zip` and `.tar` archives.

---

### 2. Development Integration

Use the `r3kit-dev` release if you want access to all headers, and prebuilt binaries:

- Add r3kit's `include` directory to your compiler's header search path
- Link against the provided `.dll` build artifact.
- Use `r3kit.h` to bootstrap the library and access the full API

---

### 3. Write Some Code
```c
#include <include/r3kit/mem/alloc.h>    // include the API's you need
#include <include/r3kit/io/str.h>

int main() {
    void* some_memory;

    // allocate 1024 bytes, 8 byte aligned
    if ((some_memory = r3_alloc(1024, 8))) {
        // code away...
    } else { return 1; } // out of memory!

    String some_string;
    if (r3_str_alloc(32, &some_string)) {
        // string code here...
    } else {
        r3_dealloc(some_memory);    // free on error...
        return 1;
    }

    // don't froget to free...
    r3_str_dealloc(some_string);
    r3_dealloc(some_memory);
    return 0;
}
```

---

## Building From Source

### Option 1: Use your preferred build system or a minimal setup:

```bash
gcc -shared -o r3kit.dll src/r3kit/*.c
```

### Option 2: The build process can be automated using [`r3make`](https://github.com/r3shape/r3make).

```bash
r3make -nf
```
| <b>NOTE:</b> This command builds the r3kit library

## r3kit Contributors

<a href="https://github.com/r3shape/r3kit/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=r3shape/r3kit" />
</a>

## License

r3kit is released under the MIT License.

