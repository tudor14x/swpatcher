#include <iostream>
#include <fstream>
#include <string.h>
#include <array>
#include <string>
#include <algorithm>
#include <filesystem>
#include <cstdarg>
#include <set>

namespace fs = std::filesystem;

std::string fixPath(const std::string &strPath)
{
    return fs::path(strPath).make_preferred().string();
}

std::string executableName;
#define DEFAULT_SUFFIX "_patched"

void help(int code = 0, const char *format = NULL, ...)
{
    std::cout << "usage: " << executableName << " --appid=<id> [--executablePath=<path>] | [--absoluteWorkshopPath=<path> && --absoluteExecutablePath=<path>] ; [--suffix=<string>]\n";
    std::cout << "\tappid                  (integer) = the appid of the steam game you're patching\n";
    std::cout << "\texecutablePath         (path)    = the relative executable path. use only when " << executableName << " is in the root folder of the game\n";
    std::cout << "\tabsoluteWorkshopPath   (path)    = the (non-relative) path to `steamapps/workshop/content/`\n";
    std::cout << "\tabsoluteExecutablePath (path)    = the (non-relative) path to the game executable\n";
    std::cout << "\tsuffix                 (string)   = name appended to the end of a directory to mark it as to be copied; default value is `" << DEFAULT_SUFFIX << "`\n\n";
    std::cout << executableName << " works by copying the `steamapps/workshop/content/<appid>[suffix]` directory into `steamapps/workshop/content/[appid]` and then launching the game; this is useful for when you make local changes to workshop content and don't want them to be overriden the next time you open the game.\n\n";

    if (!format)
    {
        std::cout << "\n\n";
        exit(0);
    }

    va_list args;
    va_start(args, format);

    char buf[255];
    auto ec = vsprintf(buf, format, args);
    if (ec < 0)
    {
        std::cerr << "formatting error\n";
    }
    else
    {
        std::cout << buf;
    }

    va_end(args);

    std::cout << "\n\n";
    exit(0);
}

const std::string argNames[] = {
    "executablePath",
    "appid",
    "absoluteWorkshopPath",
    "absoluteExecutablePath",
    "suffix"};

/* to prevent duplicates */
std::set<std::string> argsUsed = {};

int main(int argc, char **argv)
{
    executableName.assign(argv[0]);
    executableName = fs::path(executableName).stem().string();

    argc -= 1;
    argv++;

    if (argc < 2)
    {
        help();
    }

    std::string executablePath, workshopPath, suffix = DEFAULT_SUFFIX;
    short absolutePathsSet = 0;
    long long appid = 0;

    for (int i = 0; i < argc; ++i)
    {
        std::string arg(argv[i]);
        if (arg.substr(0, 2) != "--")
            help(-1, "error: invalid argument format: missing `--`");
        arg.assign(arg.data() + 2);
        size_t eqpos = arg.find('=');
        if (eqpos == std::string::npos)
            help(-1, "error: invalid argument format: missing `=` after argument name or `--`\n");
        std::string argName = arg.substr(0, eqpos);
        if (argName.size() == 0)
            help(-1, "error: invalid argument format: argument name is empty\n");
        if (std::find(std::begin(argNames), std::end(argNames), argName) == std::end(argNames))
            help(-1, "Invalid argument `%s`\n", argName.c_str());
        arg.assign(arg.data() + argName.size() + 1);
        std::string argValue = arg;

        if (argsUsed.count(argName) != 0)
        {
            std::cerr << "error: duplicate argument `" << argName << "`\n";
            return -1;
        }
        argsUsed.insert(argName);

        if (argName == "executablePath")
        {
            executablePath = argValue;
        }
        else if (argName == "appid")
        {
            appid = atoll(argValue.c_str());
        }
        else if (argName == "absoluteExecutablePath")
        {
            executablePath = argValue;
        }
        else if (argName == "absoluteWorkshopPath")
        {
            workshopPath = argValue;
        }
        else if (argName == "suffix")
        {
            suffix = argValue;
        }
    }

    if (!fs::exists(fs::path(executablePath)))
    {
        std::cerr << "error: no such file or directory `" << executablePath << "` exists\n";
        return -1;
    }

    if (workshopPath.empty())
    {
        workshopPath = fixPath(std::filesystem::current_path().parent_path().parent_path().string() + "\\workshop\\content");
    }
    auto originalPath = fixPath(workshopPath + "\\" + std::to_string(appid));
    auto patchedPath = fixPath(originalPath + suffix);

    namespace fs = std::filesystem;

    fs::recursive_directory_iterator it(patchedPath);
    fs::recursive_directory_iterator end;

    std::cout << "copying files...\n";

    for (; it != end; ++it)
    {
        const auto &entry = *it;

        // Skip .git and everything inside it
        if (entry.is_directory() && entry.path().filename() == ".git")
        {
            it.disable_recursion_pending();
            continue;
        }

        auto relative = fs::relative(entry.path(), patchedPath);
        auto target = originalPath / relative;

        if (entry.is_directory())
        {
            fs::create_directories(target);
        }
        else if (entry.is_regular_file())
        {
            fs::create_directories(target.parent_path());

            fs::copy_file(
                entry.path(),
                target,
                fs::copy_options::overwrite_existing);
        }
    }

    std::cout << "copied files. launching game\n";

    executablePath = "\"" + executablePath + "\"";

    std::string cmdString;
#ifdef _WIN32
    cmdString = std::string("start .\\") + executablePath;
#endif // _WIN32
#ifdef __linux__
    cmdString = executablePath + std::string(" & ");
#endif // __linux__
    (void)system(cmdString.c_str());

    return 0;
}