<h1>
  <img src="assets/logo.png" alt="Dominion Minion Logo" width="200" style="vertical-align: middle;">
  doMINION
</h1>

## Installation

### Ubuntu 24.04

First, install all dependencies of the project:
```
apt-get update && apt-get install \
    cmake \
    libwxgtk3.0-gtk3-dev
```

If you also want the development dependencies:
```
apt-get update && apt-get install \
    clang \
    clang-tidy \
    cppcheck
```

Then you can build the project with
```
mkdir -p build
cd build
cmake ..
make
```

### Ubuntu 22.04

We provide a script that does all the work for you.
The script will install all dependencies and build the project.
Just run the following command:
```
./scripts/ubuntu_setup.sh
```

### Other Platforms

The project should also work on other UNIX-like systems.
However, we only provide instructions for Ubuntu 22.04 and 24.04.
We have also tested the project on
 - macOS
 - Arch Linux
 - Manjaro Linux

## Running the Project

After building the project, 2 executables are created:
 - `client_exe`
 - `server_exe`

To get a list of all available options, run either of the executables with the
`--help` flag.

### Running Locally

If you want to test the project locally, you can run the server and client on
the same machine or on different machines in the same network.

First, start the server:
```bash
./server_exe
```

Then, start anywhere from 2 to 4 clients
```bash
./client_exe
```

### Running on `se.nicolabruhin.com`

If you like, you can also play a round of Dominion on our server.
To do so, start the client with the following command:
```bash
./client_exe
```

You will be prompted to enter the server address.
Just enter `se.nicolabruhin.com` and you are good to go.

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
