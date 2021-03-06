// -*- Mode: c++; c-basic-offset: 4; tab-width: 4; -*-

/****************************************************************************** 
 * 
 *  file:  StdOutput.h
 * 
 *  Copyright (c) 2004, Michael E. Smoot
 *  All rights reverved.
 * 
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *  
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.  
 *  
 *****************************************************************************/

#ifndef TCLAP_STDCMDLINEOUTPUT_H
#define TCLAP_STDCMDLINEOUTPUT_H

#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "Arg.hpp"
#include "CmdLineInterface.hpp"
#include "CmdLineOutput.hpp"
#include "XorHandler.hpp"



namespace TCLAP {

/**
 * A class that isolates any output from the CmdLine object so that it
 * may be easily modified.
 */
class StdOutput : public CmdLineOutput {

  public:
    /**
		 * Prints the usage to stdout.  Can be overridden to 
		 * produce alternative behavior.
		 * \param _cmd - The CmdLine object the output is generated for. 
		 */
    void usage(CmdLineInterface& _cmd) override;

    /**
		 * Prints the version to stdout. Can be overridden 
		 * to produce alternative behavior.
		 * \param _cmd - The CmdLine object the output is generated for. 
		 */
    void version(CmdLineInterface& _cmd) override;

    /**
		 * Prints (to stderr) an error message, short usage 
		 * Can be overridden to produce alternative behavior.
		 * \param _cmd - The CmdLine object the output is generated for. 
		 * \param e - The ArgException that caused the failure. 
		 */
    void failure(CmdLineInterface& _cmd,
        ArgException& e) override;

  protected:
    /**
         * Writes a brief usage message with short args.
		 * \param _cmd - The CmdLine object the output is generated for. 
         * \param os - The stream to write the message to.
         */
    void _shortUsage(CmdLineInterface& _cmd, std::ostream& os) const;

    /**
		 * Writes a longer usage message with long and short args, 
		 * provides descriptions and prints message.
		 * \param _cmd - The CmdLine object the output is generated for. 
		 * \param os - The stream to write the message to.
		 */
    void _longUsage(CmdLineInterface& _cmd, std::ostream& os) const;

    /**
		 * This function inserts line breaks and indents long strings 
		 * according the  params input. It will only break lines at spaces, 
		 * commas and pipes.
		 * \param os - The stream to be printed to.
		 * \param s - The string to be printed.
		 * \param maxWidth - The maxWidth allowed for the output line. 
		 * \param indentSpaces - The number of spaces to indent the first line. 
		 * \param secondLineOffset - The number of spaces to indent the second
		 * and all subsequent lines in addition to indentSpaces.
		 */
    void spacePrint(std::ostream& os,
        const std::string& s,
        int maxWidth,
        int indentSpaces,
        int secondLineOffset) const;
};

inline void StdOutput::version(CmdLineInterface& _cmd) {
    const std::string& progName = _cmd.getProgramName();
    const std::string& xversion = _cmd.getVersion();

    std::cout << '\n'
              << progName << "  version: "
              << xversion << '\n'
              << '\n';
}

inline void StdOutput::usage(CmdLineInterface& _cmd) {
    std::cout << "\nUSAGE: \n\n";
    _shortUsage(_cmd, std::cout);
    std::cout << "\n\nWhere: \n\n";
    _longUsage(_cmd, std::cout);
    std::cout << '\n';
}

inline void StdOutput::failure(CmdLineInterface& _cmd,
    ArgException& e) {
    std::string progName = _cmd.getProgramName();

    std::cerr << "PARSE ERROR: " << e.argId() << '\n'
              << "             " << e.error() << '\n'
              << '\n';

    if (_cmd.hasHelpAndVersion()) {
        std::cerr << "Brief USAGE: \n";

        _shortUsage(_cmd, std::cerr);

        std::cerr << "\nFor complete USAGE and HELP type: "
                  << "\n   " << progName << " --help"
                  << "\n\n";
    } else {
        usage(_cmd);
    }

    throw ExitException(1);
}

inline void
StdOutput::_shortUsage(CmdLineInterface& _cmd,
    std::ostream& os) const {
    const std::vector<Arg*>& argList = _cmd.getArgList();
    const std::string& progName = _cmd.getProgramName();
    const XorHandler& xorHandler = _cmd.getXorHandler();

    std::string s = progName + ' ';

    // first the xor
    for (const auto& xorArgs : xorHandler.getXorList()) {
        s += " {";
        for (auto arg : xorArgs) {
            s += arg->shortID("val") + '|';
        }
        s[s.length() - 1] = '}';
    }

    // then the rest
    for (auto arg : argList) {
        if (!xorHandler.contains(arg)) {
            s += ' ' + arg->shortID("val");
        }
    }
    // if the program name is too long, then adjust the second line offset
    int secondLineOffset = std::min<int>(75 / 2, progName.length() + 2);

    spacePrint(os, s, 75, 3, secondLineOffset);
}

inline void
StdOutput::_longUsage(CmdLineInterface& _cmd,
    std::ostream& os) const {
    const std::string& message = _cmd.getMessage();
    const XorHandler& xorHandler = _cmd.getXorHandler();
    std::vector<std::vector<Arg*>> xorList = xorHandler.getXorList();

    // first the xor
    for (int i = 0; i < xorList.size(); i++) {
        for (auto it = xorList[i].begin();
             it != xorList[i].end();
             it++) {
            spacePrint(os, (*it)->longID("val"), 75, 3, 3);
            spacePrint(os, (*it)->getDescription(), 75, 5, 0);

            if (it + 1 != xorList[i].end()) {
                spacePrint(os, "-- OR --", 75, 9, 0);
            }
        }
        os << '\n'
           << '\n';
    }

    // then the rest
    for (auto arg : _cmd.getArgList()) {
        if (!xorHandler.contains(arg)) {
            spacePrint(os, arg->longID("val"), 75, 3, 3);
            spacePrint(os, arg->getDescription(), 75, 5, 0);
            os << '\n';
        }
    }

    os << '\n';

    spacePrint(os, message, 75, 3, 0);
}

inline void StdOutput::spacePrint(std::ostream& os,
    const std::string& s,
    const int maxWidth,
    int indentSpaces,
    const int secondLineOffset) const {
    if ((s.length() + indentSpaces > maxWidth) && maxWidth > 0) {
        long allowedLen = maxWidth - indentSpaces;
        int start = 0;
        while (start < s.length()) {
            // find the substring length
            // int stringLen = std::min<int>( s.length() - start, allowedLen );
            // doing it this way to support a VisualC++ 2005 bug
            long stringLen = std::min<long>(s.length() - start, allowedLen);

            // trim the length so it doesn't end in middle of a word
            if (stringLen == allowedLen) {
                while (stringLen >= 0 && s[stringLen + start] != ' ' && s[stringLen + start] != ',' && s[stringLen + start] != '|') {
                    stringLen--;
                }
            }

            // ok, the word is longer than the line, so just split
            // wherever the line ends
            if (stringLen <= 0) {
                stringLen = allowedLen;
            }

            // check for newlines
            for (int i = 0; i < stringLen; i++) {
                if (s[start + i] == '\n') {
                    stringLen = i + 1;
                }
            }

            // print the indent
            for (int i = 0; i < indentSpaces; i++) {
                os << ' ';
            }

            if (start == 0) {
                // handle second line offsets
                indentSpaces += secondLineOffset;

                // adjust allowed s.length()
                allowedLen -= secondLineOffset;
            }

            os << s.substr(start, stringLen) << '\n';

            // so we don't start a line with a space
            while (s[stringLen + start] == ' ' && start < s.length()) {
                start++;
            }

            start += stringLen;
        }
    } else {
        for (int i = 0; i < indentSpaces; i++) {
            os << ' ';
        }
        os << s << '\n';
    }
}

} //namespace TCLAP
#endif
