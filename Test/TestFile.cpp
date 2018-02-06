#include "TestFile.h"
#include <fstream>

TestFile::TestFile()
{

}

void TestFile::run()
{
    std::ifstream file;
    file.open("c:\\tmp\\Тест\\ФайлUtf8.txt");

    if (!file.is_open()) {
        return;
    }

    char buffer[20];

    file.read(buffer, 20);
}
