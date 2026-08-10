# C++ Best Practices & Common Errors

When working on this project, always adhere to the following rules to prevent common compilation and build errors:

## 1. Method Name and Signature Verification
Always verify exact method names and signatures against the header files before calling them, especially after a refactor. Do not assume a method's name based on memory (e.g., `isActionJustReleased` vs `isActionReleased`, or `jumpForce` vs `getJumpForce()`). Use `grep_search` or `view_file` to confirm.

## 2. Standard Library Includes
Always ensure the correct standard library headers are included in your `.cpp` files. 
- For example, when using `std::abs()` on floating-point numbers (`float`), you **must** include `<cmath>`. Omitting this will cause compilation errors or silent truncation to integer types.

## 3. CMake CONFIGURE_DEPENDS
When updating or adding to `CMakeLists.txt`, always use the `CONFIGURE_DEPENDS` flag in `file(GLOB ...)` or `file(GLOB_RECURSE ...)`.
- Example: `file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")`
- This ensures that newly created source files are automatically detected by CMake and added to the Makefiles on the next build, preventing "undefined reference to vtable" linker errors.
