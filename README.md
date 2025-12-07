# 🧩 Homework Tester Setup Guide

This guide explains how to integrate and configure the `ds-tester` framework for course 234218 homework assignments.

---

## Step 0: Correct Project Structure (Library + Executable)

#### There is an example file at ExampleCMakeLists.txt

Before the tester can work, your own project **must** have a **library**. The tester is a separate executable program
that *links* against your library to run its functions.

**Why is this required?**
This "Library + Executable" pattern is a standard practice in software development.

* **Separation:** It cleanly separates your core logic (the "library") from the code that runs it (the "executable" or,
  in this case, the *tester*).
* **Linkability:** It allows *other* programs (like our tester) to import and use your code. If you just build an
  executable, your code can't be imported and tested (_without much pain_).

In your main `CMakeLists.txt` (the one in the `homework` folder), you should define your library.

**Example `CMakeLists.txt`:**

```cmake
# Define your code as a LIBRARY.
# This compiles your .cpp files into a package, but doesn't create a runnable .exe
add_library(
        my_lib # This is the library name the tester will look for, name it however you like, but don't forget to also change the attacher
        my_file.cpp
        my_other_file.cpp
        maybe_headers.hpp
)

# IMPORTANT to keep in mind: anything required by the code must be linked to library, so that it's also exposed to the tester.
target_include_directories(my_lib PRIVATE .) # includes for example

# You can still have your OWN executable for your own debugging
add_executable(
        my_main
        main.cpp
)

# Link your library's code into your own executable
target_link_libraries(my_main PRIVATE my_lib)
````

**Key takeaway:** The tester setup *needs* a target named `my_lib` to exist, which you create using
`add_library()`.

-----

## Step 1: Setup the Fetcher

Inside `homework/CMakeLists.txt`, the first job is to download (or *fetch*) the tester files from GitHub. This is
handled by the `FetchContent` module.

```cmake
include(FetchContent)
FetchContent_Declare(ds_tester
        GIT_REPOSITORY https://github.com/jamalabo1/technion-utils.git
        GIT_TAG 234218-v1-latest # replace {major} with the desired test "batch?" (0, 1, 2, etc...)
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(ds_tester)
```

### Parameter Explanation

* `FetchContent_Declare(ds_tester ...)`: This command defines a new content "package" to be fetched and names it
  `ds_tester`.
    * `GIT_REPOSITORY`: The URL of the code repository.
    * `GIT_TAG`: This is the **most important** parameter. It specifies the *exact version* of the tests to download.
        * `234218-v0-latest`: This tag breaks down as `[CourseID]-v[HW_Number]-latest`. It ensures you get the latest
          tests and bug fixes for **Homework {Major}**.
    * `GIT_SHALLOW TRUE`: An optimization that downloads only that specific version, not the entire project history,
      making the download much faster.
    * > **To get test updates:** You must delete your `build/_deps` folder and re-run `cmake`.
* `FetchContent_MakeAvailable(ds_tester)`: This command actually performs the download (if not cached) and makes the
  tester's code available to our project.

-----

## Step 2: Attach the Tester

After the tester code is downloaded, this one-liner connects it to your library.

```cmake
# attach tests
ds_tester_attach(my_lib HW hw0)
```

This `ds_tester_attach` function (which comes from the downloaded code) automatically creates the `run_tests` target and
links it against your code.

### Parameter Explanation

1. `my_lib`: This **must match the library name** you defined in **Step 0**. This tells the tester *what* to test.
2. `HW`: The *type* of assignment. This is for the tester's internal use.
3. `hw0`: The *name* of the assignment. This tells the tester which specific set of test files to activate (e.g., `hw0`,
   `hw1`, etc.).

_the hw flag maybe deprecated in future releases_

-----

## (Advanced) Using a Local Tester

The script also provides a way to skip the download and use a local copy of the tester. This is useful if you are
debugging the tester itself or if you are offline.

This is controlled by the `if/else` block at the beginning of the file.

```cmake
# if you want to customize the tester or any other reason, use can provide the path to the tester proj
set(DS_TESTER_PATH "" CACHE PATH "Use local ds-tester instead of fetching")

# check and safe guard
if (DS_TESTER_PATH AND EXISTS "${DS_TESTER_PATH}/CMakeLists.txt")
    # If path is valid, add it directly
    message(STATUS "using local dev dir: ${DS_TESTER_PATH}")
    add_subdirectory("${DS_TESTER_PATH}" _ds_tester)
else ()
    # Otherwise, download it (as shown in Step 1)
    include(FetchContent)
    # ...
    FetchContent_MakeAvailable(ds_tester)
endif ()
```

### How to Use It

You set the `DS_TESTER_PATH` variable when you run CMake for the *first time*.

1. Clone the `technion-utils` repository somewhere on your computer:
   `git clone https://github.com/jamalabo1/technion-utils.git /path/to/my/local-tester`

2. Run `cmake` from your `build` directory, pointing to that path:

   ```bash
   # Note the -D to define the variable
   cmake -DDS_TESTER_PATH=/path/to/my/local-tester ..
   ```

The script will detect this path, print "using local dev dir," and completely skip the `FetchContent` download steps.

you could also use docker with image `jamalabo1/technion-utils:234218` for a ready docker image with cmake configured,
then you could use mounts for the local tester