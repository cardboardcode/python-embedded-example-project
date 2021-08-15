#include <libgen.h> // Needed by readlink

#include <iostream>
#include <Python.h>

#include <pybind11/pybind11.h>
#include <pybind11/eval.h>
#include <pybind11/embed.h>


namespace py = pybind11;
using namespace py::literals;

///=============================================================================
std::wstring getExecutableDir() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        const auto path = std::string(dirname(result));
        return std::wstring(path.begin(), path.end());
    }
    return L"/";
}
///=============================================================================
int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    // Get executable dir and build python PATH variable
    const auto exeDir = getExecutableDir();
    const auto pythonHome = exeDir + L"/lib";
    const auto pythonPath = exeDir + L"/lib:" + exeDir + L"/../src/app";

    std::wcout << "[ exeDir ]" << std::endl << exeDir << std::endl;
    std::wcout << "[ pythonHome ]" << std::endl << pythonHome << std::endl;
    std::wcout << "[ pythonPath ]" << std::endl << pythonPath << std::endl;

    // Initialize python
    Py_OptimizeFlag = 1;
    Py_SetProgramName(L"PythonEmbeddedExample");
    Py_SetPath(pythonPath.c_str());
    Py_SetPythonHome(pythonHome.c_str());

    std::wcout << "Python PATH set to: " << pythonPath << std::endl;

    try {
        py::scoped_interpreter guard{};

        // Disable build of __pycache__ folders
        py::exec(R"(
            import sys
            sys.dont_write_bytecode = True
        )");

        // This imports example.py from app/example.py
        // The app folder is the root folder so you don't need to specify app.example.
        // The app/example script that is being imported is from the actual build folder!
        // Cmake will copy the python scripts after you have compiled the source code.
        std::cout << "Importing module..." << std::endl;
        auto example = py::module::import("example");

        std::cout << "Initializing class..." << std::endl;
        const auto myExampleClass = example.attr("Example");
        auto myExampleInstance = myExampleClass("Hello World"); // Calls the constructor
        // Will print in the terminal window:
        // Example constructor with msg: Hello World

        const auto msg = myExampleInstance.attr("getMsg")(); // Calls the getMsg
        std::cout << "Got msg back on C++ side: " << msg.cast<std::string>() << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Something went wrong: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
