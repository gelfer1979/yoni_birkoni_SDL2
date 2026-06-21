#include <iostream>
#include <sys/stat.h>
#include <string>

std::string normalize_path(const std::string& path) {
    std::string s = path;
    for (char& c : s) {
        if (c == '\\') {
            c = '/';
        }
    }
    return s;
}

int main() {
    std::string path1 = "1\\";
    std::string norm1 = normalize_path(path1);
    struct stat st;
    int res = stat(norm1.c_str(), &st);
    std::cout << "Path: " << path1 << " -> Normalized: " << norm1 << " -> stat res: " << res << std::endl;
    
    std::string path2 = "1";
    int res2 = stat(path2.c_str(), &st);
    std::cout << "Path: " << path2 << " -> stat res: " << res2 << std::endl;
    return 0;
}
