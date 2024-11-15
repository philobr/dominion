# doMINION

## Developing

In order to develop on the project, it is best to use the provided `Dockerfile` to create a development environment.

### Building the Docker Image

To build the Docker image, run the following command in the root of the project:
```bash
cd docker
docker compose run --build --rm dev
```

This will build the image if necessary, start a container with the project mounted at `/dominion`
and drop you into a shell in the container.
The image provides all the necessary tools to build and test the project.

### Building and testing the Project

Generate the build files:
```bash
mkdir -p build
cd build
cmake ..
```

In the build directory you can now do the following:

Build the project:
```bash
cmake --build .
```

Run the tests:
```bash
cmake --build . --target test
```

Format the code (`clang-format`):
```bash
cmake --build . --target format
```

Run the linter (`cppcheck`):
```bash
cmake --build . --target check
```

### Before Pushing

Before pushing your changes, make sure to:
 - Run the tests
 - Format the code
 - Run the linter

## Documentation

Documentation is provided in form of a Doxyfile.

### How to
Create the documentation:
```
doxygen doxygen/Doxyfile
```

Open the documentation:
- on OSX:
    ```
    open doxygen/html/index.html
    ```
- on linux:
    ```
    xdg-open doxygen/html/index.html
    ```

### Debugging
- `warning: Graphviz is not installed or found.`
    - Ubuntu/Debian:
        ```bash
        sudo apt install graphviz
        ```
    - OSX:
        ```bash
        brew install graphviz
        ```
- `warning: pdflatex not found, unable to generate LaTeX output`
    - Ubuntu/Debian:
       ```
       sudo apt install graphviz
       ```
    - OSX
        ```
        brew install graphviz
        ```