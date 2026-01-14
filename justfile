test name:
    cmake --build debug -t {{ name }} -j 1
    debug/Tests/{{ name }}
