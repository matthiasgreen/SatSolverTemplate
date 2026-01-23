test name:
    cmake --build debug -t {{ name }} -j 1
    ./debug/Tests/{{ name }}


test_coverage name:
    cmake --build debug_coverage -t {{ name }} -j 1
    ./debug_coverage/Tests/{{ name }}
