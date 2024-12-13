<h1>
  <img src="assets/logo.png" alt="Dominion Minion Logo" width="200" style="vertical-align: middle;">
  doMINION
</h1>

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

Documentation is provided using Doxygen.

### How to Generate the Documentation

To create the documentation, run the following command from the root directory:

```bash
(cd doxygen && doxygen Doxyfile)
```

### Open the Documentation

After generating, open the documentation:

- On macOS:
    ```bash
    open doxygen/html/index.html
    ```
- On Linux:
    ```bash
    xdg-open doxygen/html/index.html
    ```

### Installing Doxygen

If Doxygen is not installed on your system, install it using the following commands:

- **Ubuntu/Debian**:
    ```bash
    sudo apt install doxygen
    ```
- **macOS**:
    ```bash
    brew install doxygen
    ```

### Debugging

If you encounter issues, here are some common fixes:

#### `warning: Graphviz is not installed or found.`
Graphviz is required for generating diagrams. Install it using:

- **Ubuntu/Debian**:
    ```bash
    sudo apt install graphviz
    ```
- **macOS**:
    ```bash
    brew install graphviz
    ```

#### `warning: pdflatex not found, unable to generate LaTeX output`
LaTeX is required for PDF documentation generation. Install it using:

- **Ubuntu/Debian**:
    ```bash
    sudo apt install texlive-latex-base texlive-latex-extra texlive-fonts-recommended
    ```
- **macOS**:
    ```bash
    brew install --cask mactex
    ```
