#include <iostream>
#include <Windows.h>
#include <csignal>
#include <map>
#include <sstream>
#include "lexer.h"

#define FIREDLL_VERSION "1.0"

typedef void* (*voidDLLProc)();
typedef void* (*dllFunc)(char*...);

void writeOutput(const char* dllProc, const char* type, void* output) {
    std::cout << dllProc << " = ";

    if(strcmp(type, "char*") == 0)
        std::cout << *(char*)output;
    else if(strcmp(type, "wchar*") == 0)
        std::cout << *(wchar_t*)output;
    else if(strcmp(type, "byte*") == 0)
        std::cout << *(unsigned char*)output;
    else if(strcmp(type, "short*") == 0)
        std::cout << *(short*)output;
    else if(strcmp(type, "ushort*") == 0)
        std::cout << *(unsigned short*)output;
    else if(strcmp(type, "int*") == 0)
        std::cout << *(int*)output;
    else if(strcmp(type, "uint*") == 0)
        std::cout << *(unsigned int*)output;
    else if(strcmp(type, "long*") == 0)
        std::cout << *(__int64*)output;
    else if(strcmp(type, "ulong*") == 0)
        std::cout << *(unsigned __int64*)output;
    else if(strcmp(type, "bool*") == 0)
        std::cout << ((*(bool *) output) ? "true" : "false");
    else if(strcmp(type, "string*") == 0)
        std::cout << (char*)output;
    else if(strcmp(type, "wstring*") == 0)
        std::cout << (wchar_t*)output;
    if(strcmp(type, "char") == 0) {
        char data;
        std::memcpy(&data, &output, sizeof(char));
        std::cout << data;
    }
    else if(strcmp(type, "wchar") == 0) {
        wchar_t data;
        std::memcpy(&data, &output, sizeof(wchar_t));
        std::cout << data;
    }
    else if(strcmp(type, "byte") == 0) {
        unsigned char data;
        std::memcpy(&data, &output, sizeof(unsigned char));
        std::cout << data;
    }
    else if(strcmp(type, "short") == 0) {
        short data;
        std::memcpy(&data, &output, sizeof(short));
        std::cout << data;
    }
    else if(strcmp(type, "ushort") == 0) {
        unsigned short data;
        std::memcpy(&data, &output, sizeof(unsigned short));
        std::cout << data;
    }
    else if(strcmp(type, "int") == 0) {
        int data;
        std::memcpy(&data, &output, sizeof(int));
        std::cout << data;
    }
    else if(strcmp(type, "uint") == 0) {
        unsigned int data;
        std::memcpy(&data, &output, sizeof(unsigned int));
        std::cout << data;
    }
    else if(strcmp(type, "long") == 0) {
        __int64 data;
        std::memcpy(&data, &output, sizeof(__int64));
        std::cout << data;
    }
    else if(strcmp(type, "ulong") == 0) {
        unsigned __int64 data;
        std::memcpy(&data, &output, sizeof(unsigned __int64));
        std::cout << data;
    }
    else if(strcmp(type, "bool") == 0){
        bool data;
        std::memcpy(&data, &output, sizeof(bool));
        std::cout << data;
    }
    else if(strcmp(type, "ptr") == 0)
        std::cout << output;

    std::cout << std::endl;
}

std::string TranslateError(DWORD error) {
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 nullptr,
                                 error,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 (LPSTR)&messageBuffer,
                                 0,
                                 nullptr
    );

    std::string errorString(messageBuffer, size);
    LocalFree(messageBuffer);
    return errorString;
}

void except(int) {
    throw std::exception("DLL procedure call caused an Access Violation Exception!");
}

HMODULE LoadDLL(const char* name) {
    HMODULE dll = LoadLibrary(name);
    if(dll == nullptr) {
        DWORD err = GetLastError();
        std::cerr << "Failed to load DLL: " << TranslateError(err) << std::endl;
        exit(EXIT_FAILURE);
    }

    return dll;
}

FARPROC LoadProc(HMODULE dll, const char* procName) {
    FARPROC proc = GetProcAddress(dll, procName);
    if(proc == nullptr) {
        DWORD err = GetLastError();
        std::cerr << "Failed to get procedure address from DLL: " << TranslateError(err) << std::endl;
        exit(EXIT_FAILURE);
    }

    return proc;
}

int main(int argc, char** argv) {
    if(argc == 1) {
        std::cerr << "usage: FireDLL.exe <dll path> <exported function> <return type> <args...>" << std::endl;
        std::cerr << "For more information, run FireDLL.exe /?" << std::endl;
        return EXIT_FAILURE;
    }

    if(strcmp(argv[1], "/?") == 0) {
        std::cout << "FireDLL" << std::endl;
        std::cout << "Copyright (c) Megaaabytes 2023, licensed under the MIT License." << std::endl;
        std::cout << "Source code: https://github.com/Megaaabytes/FireDLL" << std::endl;
        std::cout << "Version: " << FIREDLL_VERSION << std::endl;
        std::cout << '\n';
        std::cout << "Run exported functions from DLLs from your command line." << std::endl;
        std::cout << "usage: FireDLL.exe <dll path> <exported function> <return type> <args...>" << std::endl;
        return EXIT_SUCCESS;
    }

    try {
#ifndef NDEBUG
        signal(SIGSEGV, except);
#endif
        if(argc < 4) {
            std::cerr << "usage: FireDLL.exe <dll path> <exported function> <return type> <args...>" << std::endl;
            std::cerr << "For more information, run FireDLL.exe /?" << std::endl;
            return EXIT_FAILURE;
        }
        else if(argc == 4) {
            HMODULE dll = LoadDLL(argv[1]);
            FARPROC proc = LoadProc(dll, argv[2]);

            void* output = (voidDLLProc) proc();
            writeOutput(argv[2], argv[3], output);
            FreeLibrary(dll);
        }
        else if(argc > 4) {
            HMODULE dll = LoadDLL(argv[1]);
            FARPROC proc = LoadProc(dll, argv[2]);
            std::map<uint32_t, char*> args;
            auto dllProc = (dllFunc) proc;

            std::string argsStr;
            for(size_t i = 4; i < argc; ++i) {
                std::string str(argv[i], strlen(argv[i]));
                bool quotation = false;

                for(char c : str) {
                    if(c == 0x20) {
                        quotation = true;
                        break;
                    }
                }

                if(quotation) {
                    argsStr += '"';
                    argsStr += argv[i];
                    argsStr += '"';
                    argsStr += (char)0x20;
                }
                else {
                    argsStr += argv[i];
                    argsStr += (char)0x20;
                }
            }
            
            Lexer lexer(argsStr);
            std::vector<Token*> tokens = lexer.GetTokens();

            for(size_t i = 0; i < tokens.size(); ++i) {
                Token* token = tokens[i];

                if(token->tokenType == Token::ID) {
                    char* ptr = new char[token->data.length()];
                    for(size_t y = 0; y < token->data.length(); ++y) {
                        ptr[y] = token->data[y];
                    }
                    ptr[token->data.length()] = 0;

                    args[i] = ptr;
                }
                else if(token->tokenType == Token::DIGIT) {
                    auto digit = std::stoull(token->data, nullptr, 0);
                    if(digit <= UINT8_MAX) {
                        args[i] = reinterpret_cast<char*>((uint8_t)digit);
                    }
                    else if(digit <= UINT16_MAX) {
                        args[i] = reinterpret_cast<char*>((uint16_t)digit);
                    }
                    else if(digit <= UINT32_MAX) {
                        args[i] = reinterpret_cast<char*>((uint32_t)digit);
                    }
                    else if(digit <= UINT64_MAX) {
                        args[i] = reinterpret_cast<char*>((uint64_t)digit);
                    }
                    else if(digit <= INT8_MAX) {
                        args[i] = reinterpret_cast<char*>((int8_t)digit);
                    }
                    else if(digit <= INT16_MAX) {
                        args[i] = reinterpret_cast<char*>((int16_t)digit);
                    }
                    else if(digit <= INT32_MAX) {
                        args[i] = reinterpret_cast<char*>((int32_t)digit);
                    }
                    else if(digit <= INT64_MAX) {
                        args[i] = reinterpret_cast<char*>((int64_t)digit);
                    }
                }
                else if(token->tokenType == Token::NULLPTR) {
                    args[i] = nullptr;
                }
                else if(token->tokenType == Token::STRING) {
                    char* ptr = new char[token->data.length()];
                    for(size_t y = 0; y < token->data.length(); ++y) {
                        ptr[y] = token->data[y];
                    }
                    ptr[token->data.length()] = 0;

                    args[i] = ptr;
                }
                else if(token->tokenType == Token::WSTRING) {
                    auto* ptr = new wchar_t[token->data.length() + 1];
                    size_t outputSize;
                    mbstowcs_s(&outputSize, ptr, token->data.length() + 1, token->data.c_str(), token->data.length());
                    ptr[token->data.length()] = 0;

                    args[i] = reinterpret_cast<char*>(ptr);
                }
            }

            writeOutput(argv[2], argv[3], dllProc(args[0],
                             args[1],
                             args[2],
                             args[3],
                             args[4],
                             args[5],
                             args[6],
                             args[7],
                             args[8],
                             args[9],
                             args[10],
                             args[11],
                             args[12],
                             args[13],
                             args[14],
                             args[15],
                             args[16],
                             args[17],
                             args[18],
                             args[19],
                             args[20],
                             args[21],
                             args[22],
                             args[23],
                             args[24],
                             args[25],
                             args[26],
                             args[27],
                             args[28],
                             args[29],
                             args[30],
                             args[31],
                             args[32],
                             args[33],
                             args[34],
                             args[35],
                             args[36],
                             args[37],
                             args[38],
                             args[39],
                             args[40],
                             args[41],
                             args[42],
                             args[43],
                             args[44],
                             args[45],
                             args[46],
                             args[47],
                             args[48],
                             args[49],
                             args[50],
                             args[51],
                             args[52],
                             args[53],
                             args[54],
                             args[55],
                             args[56],
                             args[57],
                             args[58],
                             args[59],
                             args[60],
                             args[61],
                             args[62],
                             args[63],
                             args[64],
                             args[65],
                             args[66],
                             args[67],
                             args[68],
                             args[69],
                             args[70],
                             args[71],
                             args[72],
                             args[73],
                             args[74],
                             args[75],
                             args[76],
                             args[77],
                             args[78],
                             args[79],
                             args[80],
                             args[81],
                             args[82],
                             args[83],
                             args[84],
                             args[85],
                             args[86],
                             args[87],
                             args[88],
                             args[89],
                             args[90],
                             args[91],
                             args[92],
                             args[93],
                             args[94],
                             args[95],
                             args[96],
                             args[97],
                             args[98],
                             args[99],
                             args[100],
                             args[101],
                             args[102],
                             args[103],
                             args[104],
                             args[105],
                             args[106],
                             args[107],
                             args[108],
                             args[109],
                             args[110],
                             args[111],
                             args[112],
                             args[113],
                             args[114],
                             args[115],
                             args[116],
                             args[117],
                             args[118],
                             args[119],
                             args[120],
                             args[121],
                             args[122],
                             args[123],
                             args[124],
                             args[125],
                             args[126]
                        )
            );
        }
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch(...) {
        std::cerr << "Unknown exception was thrown during procedure call." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
