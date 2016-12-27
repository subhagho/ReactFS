//
// Created by Subhabrata Ghosh on 27/12/16.
//

#ifndef REACTFS_STYLE_H
#define REACTFS_STYLE_H

#include "common/includes/base_error.h"
#include "common/includes/log_utils.h"
#include "core/includes/core.h"

// allow for different calling conventions in Linux and Windows
#ifdef _WIN32
#define STDCALL __stdcall
#else
#define STDCALL
#endif

// functions to call AStyleMain
extern "C" const char *STDCALL AStyleGetVersion(void);
extern "C" char *STDCALL AStyleMain(const char *sourceIn,
                                    const char *optionsIn,
                                    void (STDCALL *fpError)(int, const char *),
                                    char *(STDCALL *fpAlloc)(unsigned long));

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace parsers {
                    class styler {
                    private:
                        const char *options = "-A10tOPxnN";
                    public:
                        styler() {
                            // get Artistic Style version
                            const char *version = AStyleGetVersion();
                            cout << "C++ - AStyle " << version << endl;
                        }

                        void style(const vector<string> &files) {
                            for (string file : files) {
                                style(file);
                            }
                        }

                        void style(string file) {

                            char *textIn = getText(file);
                            CHECK_NOT_NULL(textIn);

                            // call the Artistic Style formatting function
                            char *textOut = AStyleMain(textIn,
                                                       options,
                                                       ASErrorHandler,
                                                       ASMemoryAlloc);
                            // does not need to terminate on an error
                            // an error message has been displayed by the error handler
                            if (IS_NULL(textOut))
                                error(string("Error formatting " + file));
                            setText(textOut, file);

                            // must delete the temporary buffers
                            delete[] textIn;
                            delete[] textOut;
                        }

                        // Error handler for the Artistic Style formatter.
                        static void  STDCALL ASErrorHandler(int errorNumber, const char *errorMessage) {
                            throw BASE_ERROR("astyle error : [%d] %s", errorNumber, errorMessage);
                        }

                        // Allocate memory for the Artistic Style formatter.
                        static char *STDCALL ASMemoryAlloc(
                                unsigned long memoryNeeded) {   // error condition is checked after return from AStyleMain
                            char *buffer = new(nothrow) char[memoryNeeded];
                            return buffer;
                        }

                        // Error message function for this example.
                        void error(string message) {
                            throw BASE_ERROR("astyle fatal error : %s", message.c_str());
                        }

                        // Prepend the project directory to the subpath.
                        // This may need to be changed for your directory structure.
                        string getProjectDirectory(const string &subPath) {
#ifdef _WIN32
                            char* homeDirectory = getenv("USERPROFILE");
#else
                            char *homeDirectory = getenv("HOME");
#endif
                            if (!homeDirectory)
                                error("Cannot find HOME directory");
                            string projectPath = string(homeDirectory) + "/Projects/" + subPath;
                            return projectPath;
                        }

                        // Get the text to be formatted.
                        // Usually the text would be obtained from an edit control.
                        char *getText(const string &filePath) {   // open input file
                            ifstream in(filePath.c_str());
                            if (!in)
                                error("Cannot open input file " + filePath);

                            // get length of buffer
                            const int bufferSizeIn = 131072;     // 128 KB

                            // allocate memory
                            char *bufferIn = new(nothrow) char[bufferSizeIn];
                            if (bufferIn == NULL) {
                                in.close();
                                error("Memory allocation failure on input");
                            }

                            // read data as a block
                            in.read(bufferIn, bufferSizeIn);
                            // get actual size - must be smaller than buffer size
                            int textSizeIn = static_cast<int>(in.gcount());
                            if (textSizeIn > bufferSizeIn) {
                                in.close();
                                error("Read buffer is too small");
                            }
                            bufferIn[textSizeIn] = '\0';
                            in.close();

                            return bufferIn;
                        }

                        // Return the formatted text.
                        // Usually the text would be returned to an edit control.
                        void setText(const char *textOut, const string &filePathStr) {   // create a backup file
                            const char *filePath = filePathStr.c_str();
                            string origfilePathStr = filePathStr + ".orig";
                            const char *origfilePath = origfilePathStr.c_str();
                            remove(origfilePath);              // remove a pre-existing file

                            if (rename(filePath, origfilePath) < 0)
                                error("Cannot open input file " + filePathStr);

                            // open the output file
                            ofstream out(filePath);
                            if (!out)
                                error("Cannot open output file " + filePathStr);

                            // write the text
                            size_t textSizeOut = strlen(textOut);
                            out.write(textOut, static_cast<streamsize>(textSizeOut));
                            out.close();

                            remove(origfilePath);
                        }

                    };
                }
REACTFS_NS_CORE_END;
#endif //REACTFS_STYLE_H
