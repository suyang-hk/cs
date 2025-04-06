//  settings.json:
{
    "code-runner.executorMap": {
        "cpp": "cd $dir && g++ '$fileName' -o '$fileNameWithoutExt' -std=c++17 -Wall -Wpedantic -Wextra && echo 'compilation ends.' && ./'$fileNameWithoutExt'",
        "c": "cd $dir && gcc '$fileName' -o '$fileNameWithoutExt' -std=c17 -Wall -Wpedantic -Wextra && echo 'compilation ends.' && ./'$fileNameWithoutExt'"
    }
}
//  c_cpp_properties.json: （用于配置"C/C++"插件）
//  Windows:
{
    "configurations": [
        {
            "name": "Win32",
            "compilerPath": "C:\\mingw64\\bin\\gcc.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-gcc-x64",
            "compilerArgs": [
                "-Wall",
                "-Wpedantic",
                "-Wextra"
            ]
        }
    ],
    "version": 4
}
