<<<<<<< HEAD
# ![r3shape-labs](https://github.com/user-attachments/assets/ac634f13-e084-4387-aded-4679eb048cac) **r3shape's kit of tools**

**r3kt** is designed to be minimal, and extensible, making writing complex programs simpler by reducing bugs and boilerplate, helping you ship faster.
=======
# ![r3shape-labs](https://github.com/user-attachments/assets/ac634f13-e084-4387-aded-4679eb048cac)  <br> **r3shape's kit of tools**

**r3kit** is designed to be minimal, and extensible, making writing complex programs simpler by reducing bugs and boilerplate, helping you ship faster.
>>>>>>> 30b0500 ([r3kit rename :)])

---
## Features

- **Modular architecture** – Include only the APIs you need.
- **No platform dependencies** – Fully self-contained, written in C99.
<<<<<<< HEAD
- **Runtime metadata** – Track module size, version, function calls, and usage stats.
=======
- **Simple Integration** - Any api can serve as a drop-in along side the `r3kit.mem.alloc` API.
>>>>>>> 30b0500 ([r3kit rename :)])

---

## API Overview
| API     | Description                           |
|---------|---------------------------------------|
<<<<<<< HEAD
| `r3ktmem`    | Memory management |
| `r3ktarr`    | Array data structures |
| `r3ktstr`    | String handling/manipulation |
| `r3ktlog`    | Logging utilities |
| `r3ktmath`   | Math utilities (GL friendly) |
| `r3ktfile`   | File handling/manipulation |
=======
| `mem`     | Memory management |
| `ds`      | Data Structures   |
| `io`      | Input/Output      |
| `math`    | Math utilities (GL friendly) |
>>>>>>> 30b0500 ([r3kit rename :)])

---

## Getting Started

### 1. Choose a Release

| Release       | Contents                                                                 |
|---------------|--------------------------------------------------------------------------|
<<<<<<< HEAD
| `r3kt-dev`    | Full development version: headers, and prebuilt `.dll` |
| `r3kt-rt`     | Standard featureset + r3kt runtime: prebuilt `.dll` |
| `r3kt`        | Standard release: prebuilt `.dll` |
=======
| `r3kit-dev`    | Full development version: headers, and prebuilt `.dll` |
| `r3kit`        | Standard release: prebuilt `.dll` |
>>>>>>> 30b0500 ([r3kit rename :)])

| <b>NOTE:</b> Both versions are available as `.zip` and `.tar` archives.

---

### 2. Development Integration

<<<<<<< HEAD
Use the `r3kt-dev` release if you want access to all headers, and prebuilt binaries:

- Add r3kt's `include` directory to your compiler's header search path
- Link against the provided `.dll` build artifact.
- Use `r3kt.h` to bootstrap the library and access the full API
=======
Use the `r3kit-dev` release if you want access to all headers, and prebuilt binaries:

- Add r3kit's `include` directory to your compiler's header search path
- Link against the provided `.dll` build artifact.
- Use `r3kit.h` to bootstrap the library and access the full API
>>>>>>> 30b0500 ([r3kit rename :)])

---

### 3. Write Some Code
```c
<<<<<<< HEAD
#include <include/r3kt/r3ktmem.h>    // include the API's you need
=======
#include <include/r3kit/mem/alloc.h>    // include the API's you need
#include <include/r3kit/io/str.h>
>>>>>>> 30b0500 ([r3kit rename :)])

int main() {
    void* some_memory;

    // allocate 1024 bytes, 8 byte aligned
    if ((some_memory = r3_alloc(1024, 8))) {
        // code away...
    } else { return 1; } // out of memory!

<<<<<<< HEAD
    r3_dealloc(some_memory); // don't froget to free...
=======
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
>>>>>>> 30b0500 ([r3kit rename :)])
    return 0;
}
```

---

## Building From Source

### Option 1: Use your preferred build system or a minimal setup:

```bash
<<<<<<< HEAD
gcc -shared -o r3kt.dll src/r3kt/*.c
=======
gcc -shared -o r3kit.dll src/r3kit/*.c
>>>>>>> 30b0500 ([r3kit rename :)])
```

### Option 2: The build process can be automated using [`r3make`](https://github.com/r3shape/r3make).

```bash
r3make -nf
```
<<<<<<< HEAD
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
=======
| <b>NOTE:</b> This command builds the r3kit library

## r3kit Contributors

<a href="https://github.com/r3shape/r3kit/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=r3shape/r3kit" />
>>>>>>> 30b0500 ([r3kit rename :)])
</a>

## License

<<<<<<< HEAD
r3kt is released under the MIT License.
=======
r3kit is released under the MIT License.
>>>>>>> 30b0500 ([r3kit rename :)])

