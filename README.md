# ![r3shape-labs](https://github.com/user-attachments/assets/ac634f13-e084-4387-aded-4679eb048cac)  <br> **libR3**

**libR3** is designed to be a minimal, and extensible runtime library, making writing complex programs simpler by reducing bugs and boilerplate, helping you ship faster.

---
## Features

- **Modular architecture** – Include only the APIs you need.
- **No platform dependencies** – Fully self-contained, written in C99.
- **Simple Integration** - Any api can serve as a drop-in along side the `libR3.mem.mem` API.

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
| `libR3-dev`    | Full development version: headers, and prebuilt `.dll` |
| `libR3`        | Standard release: prebuilt `.dll` |

| <b>NOTE:</b> Both versions are available as `.zip` and `.tar` archives.

---

### 2. Development Integration

Use the `libR3-dev` release if you want access to all headers, and prebuilt binaries:

- Add libR3's `include` directory to your compiler's header search path
- Link against the provided `.dll` build artifact.
- Use `libR3/r3.h` to bootstrap the library and access the full API

---

### 3. Write Some Code
```c
#include <include/libR3/mem/mem.h>
#include <include/libR3/io/string.h>

int main() {
    void* someMemory;

    // allocate 1024 bytes, (8 byte aligned on 64bit systems else 4byte aligned)
    if ((someMemory = r3AllocMemory(1024))) {
        // code away...
    }

    char* someString;
    if ((someString = r3NewString(32))) {
        // string code here...
    }

    // don't froget to free...
    r3DelString(someString);
    r3FreeMemory(someMemory);

    return 0;
}
```

---

## Building From Source

### Option 1: Use your preferred build system or a minimal setup:

```bash
gcc -shared -o libR3.dll src/libR3/*/*.c
```

### Option 2: The build process can be automated using [`r3make`](https://github.com/r3shape/r3make).

```bash
r3make -nf
```
| <b>NOTE:</b> This command builds the libR3 library

## libR3 Contributors

<a href="https://github.com/r3shape/libR3/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=r3shape/libR3" />
</a>

## License

libR3 is released under the MIT License.
