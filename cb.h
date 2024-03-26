#ifndef _MASON_H_
#define _MASON_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Mainly for giving constant values.
enum Mason_Platform_Type { Mason_Platform_Type_Windows, Mason_Platform_Type_Undefined };
typedef enum Mason_Platform_Type Mason_Platform_Type;

#if defined(_WIN32)
#include <windows.h>
#undef max
#define MASON_PLATFORM_TYPE Mason_Platform_Type_Windows
#endif

// Start String
struct Mason_String {
    char* data;
    size_t size;
};
typedef struct Mason_String Mason_String;

void mason_append_string(Mason_String* string, const char* to_append);

// End String

// Start Command
struct Mason_Cmd {
    char* data;
    size_t size;
};
typedef struct Mason_Cmd Mason_Cmd;
// End Command

// Start Process
struct Mason_Proc_Info {
// #if MASON_PLATFORM_TYPE == Mason_Platform_Type_Windows
#if defined(_WIN32)
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
#endif
};
typedef struct Mason_Proc_Info Mason_Proc_Info;

// End Process

// API
void mason_run_cmd(Mason_Cmd* command);
void mason_project_rebuild_implementation(int argc, char** argv, const char* file);

#define MASON_IMPLEMENTATION
#ifdef MASON_IMPLEMENTATION

void mason_append_string(Mason_String* string, const char* to_append) {
    assert(string);

    size_t length = strlen(to_append);
    if(string->size != 0) {
        string->data = realloc(string->data, string->size + length + 1);
    } else {
        string->data = malloc(length + 1);
    }

    char* place = string->data + string->size;
    strcpy_s(place, length + 1,  to_append);
    string->size += length;
}

void mason_run_cmd(Mason_Cmd* command) {
#if MASON_PLATFORM_TYPE == Mason_Platform_Type_Windows
    Mason_Proc_Info proc_info = { 0 };

    ZeroMemory(&proc_info.si, sizeof(proc_info.si));
    proc_info.si.cb = sizeof(proc_info.si);
    ZeroMemory(&proc_info.pi, sizeof(proc_info.pi));

    if (!CreateProcess(
            NULL,          // No module name (use command line)
            command->data, // Command line
            NULL,          // Process handle not inheritable
            NULL,          // Thread handle not inheritable
            FALSE,         // Set handle inheritance to FALSE
            0,             // No creation flags
            NULL,          // Use parent's environment block
            NULL,          // Use parent's starting directory
            &proc_info.si, // Pointer to STARTUPINFO structure
            &proc_info.pi) // Pointer to PROCESS_INFORMATION structure
    ) {
        printf("Process - %s has failed to run %d \n", command->data, GetLastError());
    }

    // Wait until child process exits.
    WaitForSingleObject(proc_info.pi.hProcess, INFINITE);

    // Close process and thread handles.
    CloseHandle(proc_info.pi.hProcess);
    CloseHandle(proc_info.pi.hThread);
#endif
}

// WIN32
#if MASON_PLATFORM_TYPE == Mason_Platform_Type_Windows

int mason_impl_compare_write_file_info(const SYSTEMTIME* first, const SYSTEMTIME* second) {
    if (!first && !second) return 0;
    if (!first) return 1;
    if (!second) return -1;
    if (first->wYear != second->wYear) return first->wYear < second->wYear ? 1 : -1;
    if (first->wMonth != second->wMonth) return first->wMonth < second->wMonth ? 1 : -1;
    if (first->wDay != second->wDay) return first->wDay < second->wDay ? 1 : -1;
    if (first->wHour != second->wHour) return first->wHour < second->wHour ? 1 : -1;
    if (first->wMinute != second->wMinute) return first->wMinute < second->wMinute ? 1 : -1;
    if (first->wSecond != second->wSecond) return first->wSecond < second->wSecond ? 1 : -1;
    if (first->wMilliseconds != second->wMilliseconds) return first->wMilliseconds < second->wMilliseconds ? 1 : -1;
    return 0;
}

int mason_impl_get_last_write_time(const Mason_String* string, SYSTEMTIME* st) {
    if (!string || !st) return 0;

    printf("%s file\n", string->data);

    HANDLE h_file =
        CreateFile(string->data, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    FILETIME ft_create, ft_access, ft_write;
    if (!h_file || !GetFileTime(h_file, &ft_create, &ft_access, &ft_write)) {
        printf("Something went wrong\n");
        return 0;
    }

    SYSTEMTIME st_utc;
    FileTimeToSystemTime(&ft_write, &st_utc);
    SystemTimeToTzSpecificLocalTime(NULL, &st_utc, st);


#ifdef MASON_PRINT_DEBUG
    printf("UTC System Time format:\n");
    printf(
        "Written on: %02d/%02d/%d %02d:%02d:%02d:%02d\n",
        st->wDay,
        st->wMonth,
        st->wYear,
        st->wHour,
        st->wMinute,
        st->wSecond,
        st->wMilliseconds);
#endif

    CloseHandle(h_file);

    return 1;
}

#endif

void mason_project_rebuild_implementation(int argc, char** argv, const char* file) {
    // @TODO: memory leaks here.
    Mason_String build_exe = { 0 };
    mason_append_string(&build_exe, argv[0]);
    mason_append_string(&build_exe, ".exe");

    Mason_String build_c = { 0 };
    mason_append_string(&build_c, file);

    int needs_rebuild = 0;

    SYSTEMTIME exe_time, c_time;
    if (!mason_impl_get_last_write_time(&build_exe, &exe_time) || !mason_impl_get_last_write_time(&build_c, &c_time)) {
        needs_rebuild = 1;
    }

    int comparison = mason_impl_compare_write_file_info(&exe_time, &c_time);
    if (comparison > 0) needs_rebuild = 1;

    // make this more compiler friendly
    const char* compiler = "cl";
    const char* flags    = "/Zi";

#if 1
    if (needs_rebuild) {
        Mason_String temp_file = { 0 };
        mason_append_string(&temp_file, argv[0]);
        mason_append_string(&temp_file, "-temp.exe");

        printf("Temp file name = %s", temp_file.data);
        DeleteFile(temp_file.data);
        MoveFile(build_exe.data, temp_file.data);

        Mason_String compilation_string = { 0 };
        mason_append_string(&compilation_string, compiler);
        mason_append_string(&compilation_string, " ");
        mason_append_string(&compilation_string, flags);
        mason_append_string(&compilation_string, " ");
        mason_append_string(&compilation_string, build_c.data);

        Mason_Cmd compilation_cmd = { 0 };
        compilation_cmd.data     = compilation_string.data;
        compilation_cmd.size     = compilation_string.size;

        printf("Running \"%s\"\n", compilation_cmd.data);
        mason_run_cmd(&compilation_cmd);

        DWORD dwAttrib = GetFileAttributes(build_exe.data);
        if (dwAttrib == INVALID_FILE_ATTRIBUTES || dwAttrib & FILE_ATTRIBUTE_DIRECTORY) {
            printf("Restoring old file because of compile error\n");
            MoveFile(temp_file.data, build_exe.data);
        }

        free(temp_file.data);
        free(compilation_string.data);

        printf("Rebuild is done\n");
    } else {
        printf("No need for rebuild\n");
    }
#endif

    free(build_c.data);
    free(build_exe.data);
}

#endif

#define MASON_SETUP_REBUILD(argc, argv) mason_project_rebuild_implementation(argc, argv, __FILE__)

#endif // _MASON_H_
