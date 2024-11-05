# doMINION

## Developing

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
