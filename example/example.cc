#include <iostream>
#include <fstream>
#include <cstdlib>

// The vulnerable function as provided
void vul(short *s) {
    if(s[0] == 0x6261) // ab
        s[2] = 0x6665;  // ef

    if(s[1] == 0x6463) // cd
        if(((int *)s)[1] == 0x21216665)  // ef!!
            abort(); // trigger abort if conditions met
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    const char *filename = argv[1];

    // Open the file in binary mode
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        return 1;
    }

    // Get the length of the file
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the file content into a buffer
    char *buffer = new char[size];
    file.read(buffer, size);
    file.close();

    // Ensure the buffer has enough data to pass to vul (at least 6 bytes for 3 short)
    if (size < sizeof(short) * 3) {
        std::cerr << "Error: Not enough data in the file to pass to vul()\n";
        delete[] buffer;
        return 1;
    }

    // Cast the buffer to short* to pass to vul
    short *s = reinterpret_cast<short *>(buffer);

    // Call the vulnerable function with the data from the file
    vul(s);

    // Clean up and return success
    delete[] buffer;
    return 0;
}


