#pragma once

class Vertex {
public:
    Vertex();

    int symbol_of_vertex = -1;
    int frequency_of_vertex = 0;
    Vertex* left_child = nullptr;
    Vertex* right_child = nullptr;
};
