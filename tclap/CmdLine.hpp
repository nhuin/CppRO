// -*- Mode: c++; c-basic-offset: 4; tab-width: 4; -*-

/******************************************************************************
 *
 *  file:  CmdLine.h
 *
 *  Copyright (c) 2003, Michael E. Smoot .
 *  Copyright (c) 2004, Michael E. Smoot, Daniel Aarno.
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

#ifndef TCLAP_CMDLINE_H
#define TCLAP_CMDLINE_H

#include "MultiSwitchArg.hpp"
#include "SwitchArg.hpp"
#include "UnlabeledMultiArg.hpp"
#include "UnlabeledValueArg.hpp"

#include "HelpVisitor.hpp"
#include "IgnoreRestVisitor.hpp"
#include "VersionVisitor.hpp"
#include "XorHandler.hpp"

#include "CmdLineOutput.hpp"
#include "StdOutput.hpp"

#include "Constraint.hpp"
#include "ValuesConstraint.hpp"

#include <algorithm>
#include <cstdlib> // Needed for exit(), which isn't defined in some envs.
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace TCLAP {

/**
 * The base class that manages the command line definition and passes
 * along the parsing to the appropriate Arg classes.
 */
class CmdLine : public CmdLineInterface {
  protected:
    /**
		 * The list of arguments that will be tested against the
		 * command line.
		 */
    std::vector<Arg*> m_argList = std::vector<Arg*>();

    /**
		 * The name of the program.  Set to argv[0].
		 */
    std::string m_progName = "";

    /**
		 * A message used to describe the program.  Used in the usage output.
		 */
    std::string m_message = "";

    /**
		 * The version to be displayed with the --version switch.
		 */
    std::string m_version = "";

    /**
		 * The number of arguments that are required to be present on
		 * the command line. This is set dynamically, based on the
		 * Args added to the CmdLine object.
		 */
    int m_numRequired = 0;

    /**
		 * The character that is used to separate the argument flag/name
		 * from the value.  Defaults to ' ' (space).
		 */
    char m_delimiter;

    /**
		 * The handler that manages xoring lists of args.
		 */
    XorHandler m_xorHandler = XorHandler();

    /**
		 * A list of Args to be explicitly deleted when the destructor
		 * is called.  At the moment, this only includes the three default
		 * Args.
		 */
    std::vector<std::unique_ptr<Arg>> m_argDeleteOnExitList = std::vector<std::unique_ptr<Arg>>();

    /**
		 * A list of Visitors to be explicitly deleted when the destructor
		 * is called.  At the moment, these are the Vistors created for the
		 * default Args.
		 */
    std::vector<std::unique_ptr<Visitor>> m_visitorDeleteOnExitList = std::vector<std::unique_ptr<Visitor>>();

    /**
		 * Object that handles all output for the CmdLine.
		 */
    std::unique_ptr<CmdLineOutput> m_output = std::make_unique<StdOutput>();

    /**
		 * Should CmdLine handle parsing exceptions internally?
		 */
    bool m_handleExceptions = true;

    /**
		 * Throws an exception listing the missing args.
		 */
    void missingArgsException() const;

    /**
		 * Checks whether a name/flag string matches entirely matches
		 * the Arg::blankChar.  Used when multiple switches are combined
		 * into a single argument.
		 * \param s - The message to be used in the usage.
		 */
    bool _emptyCombined(const std::string& s);

    /**
		 * Perform a delete ptr; operation on ptr when this object is deleted.
		 */
    void deleteOnExit(Arg* ptr);

    /**
		 * Perform a delete ptr; operation on ptr when this object is deleted.
		 */
    void deleteOnExit(Visitor* ptr);

  private:
    /**
		 * Encapsulates the code common to the constructors
		 * (which is all of it).
		 */
    void _constructor();

    /**
		 * Is set to true when a user sets the output object. We use this so
		 * that we don't delete objects that are created outside of this lib.
		 */
    bool m_userSetOutput = false;

    /**
		 * Whether or not to automatically create help and version switches.
		 */
    bool m_helpAndVersion;

  public:
    /**
		 * Command line constructor. Defines how the arguments will be
		 * parsed.
		 * \param _m - The message to be used in the usage
		 * output.
		 * \param _delim - The character that is used to separate
		 * the argument flag/name from the value.  Defaults to ' ' (space).
		 * \param _v - The version number to be used in the
		 * --version switch.
		 * \param _help - Whether or not to create the Help and
		 * Version switches. Defaults to true.
		 */
    explicit CmdLine(std::string _m,
        char _delim = ' ',
        std::string _v = "none",
        bool _help = true);

    /**
		 * Prevent accidental copying.
		 */
    CmdLine(const CmdLine& rhs) = delete;
    CmdLine& operator=(const CmdLine& rhs) = delete;
    CmdLine(CmdLine&& rhs) = default;
    CmdLine& operator=(CmdLine&& rhs) = default;

    /**
		 * Deletes any resources allocated by a CmdLine object.
		 */
    ~CmdLine() override;

    /**
		 * Adds an argument to the list of arguments to be parsed.
		 * \param a - Argument to be added.
		 */
    void add(Arg& a);

    /**
		 * An alternative add.  Functionally identical.
		 * \param a - Argument to be added.
		 */
    void add(Arg* a) override;

    /**
		 * Add two Args that will be xor'd.  If this method is used, add does
		 * not need to be called.
		 * \param a - Argument to be added and xor'd.
		 * \param b - Argument to be added and xor'd.
		 */
    void xorAdd(Arg* a, Arg* b) override;

    /**
		 * Add a list of Args that will be xor'd.  If this method is used,
		 * add does not need to be called.
		 * \param ors - List of Args to be added and xor'd.
		 */
    void xorAdd(const std::vector<Arg*>& ors) override;

    /**
		 * Parses the command line.
		 * \param argc - Number of arguments.
		 * \param argv - Array of arguments.
		 */
    void parse(int argc, const char* const* argv) override;

    /**
		 * Parses the command line.
		 * \param args - A vector of strings representing the args.
		 * args[0] is still the program name.
		 */
    void parse(std::vector<std::string>& args);

    /**
		 *
		 */
    const CmdLineOutput& getOutput() const override;

    /**
		 *
		 */
    void setOutput(CmdLineOutput* co) override;

    /**
		 *
		 */
    const std::string& getVersion() const override;

    /**
		 *
		 */
    const std::string& getProgramName() const override;

    /**
		 *
		 */
    const std::vector<Arg*>& getArgList() const override;

    /**
		 *
		 */
    const XorHandler& getXorHandler() const override;

    /**
		 *
		 */
    char getDelimiter() const override;

    /**
		 *
		 */
    const std::string& getMessage() const override;

    /**
		 *
		 */
    bool hasHelpAndVersion() const override;

    /**
		 * Disables or enables CmdLine's internal parsing exception handling.
		 *
		 * @param state Should CmdLine handle parsing exceptions internally?
		 */
    void setExceptionHandling(bool state);

    /**
		 * Returns the current state of the internal exception handling.
		 *
		 * \retval true Parsing exceptions are handled internally.
		 * \retval false Parsing exceptions are propagated to the caller.
		 */
    bool getExceptionHandling() const;

    /**
		 * Allows the CmdLine object to be reused.
		 */
    void reset() override;
};

///////////////////////////////////////////////////////////////////////////////
//Begin CmdLine.cpp
///////////////////////////////////////////////////////////////////////////////

inline CmdLine::CmdLine(std::string _m,
    char _delim,
    std::string _v,
    bool _help)
    :

    m_message(std::move(_m))
    , m_version(std::move(_v))
    , m_delimiter(_delim)
    ,

    m_helpAndVersion(_help) {
    _constructor();
}

inline CmdLine::~CmdLine() = default;

inline void CmdLine::_constructor() {
    Arg::setDelimiter(m_delimiter);
    if (m_helpAndVersion) {
        m_visitorDeleteOnExitList.push_back(std::make_unique<HelpVisitor>(*this, *m_output));
        m_argDeleteOnExitList.push_back(std::make_unique<SwitchArg>("h", "help",
            "Displays usage information and exits.",
            false, m_visitorDeleteOnExitList.back().get()));
        add(m_argDeleteOnExitList.back().get());

        m_visitorDeleteOnExitList.push_back(std::make_unique<VersionVisitor>(*this, *m_output));
        m_argDeleteOnExitList.push_back(std::make_unique<SwitchArg>("", "version",
            "Displays version information and exits.",
            false, m_visitorDeleteOnExitList.back().get()));
        add(m_argDeleteOnExitList.back().get());
    }

    m_visitorDeleteOnExitList.push_back(std::make_unique<IgnoreRestVisitor>());
    m_argDeleteOnExitList.push_back(std::make_unique<SwitchArg>(Arg::flagStartString(),
        Arg::ignoreNameString(),
        "Ignores the rest of the labeled arguments following this flag.",
        false, m_visitorDeleteOnExitList.back().get()));
    add(m_argDeleteOnExitList.back().get());
}

inline void CmdLine::xorAdd(const std::vector<Arg*>& ors) {
    m_xorHandler.add(ors);

    for (auto arg : ors) {
        arg->forceRequired();
        arg->setRequireLabel("OR required");
        add(arg);
    }
}

inline void CmdLine::xorAdd(Arg* a, Arg* b) {
    xorAdd({a, b});
}

inline void CmdLine::add(Arg* a) {
    if (std::find(m_argList.begin(), m_argList.end(), a) != m_argList.end()) {
        throw(SpecificationException(
            "Argument with same flag/name already exists!",
            a->longID("val")));
    }
    a->addToList(m_argList);

    if (a->isRequired()) {
        ++m_numRequired;
    }
}

inline void CmdLine::parse(int argc, const char* const* argv) {
    // this step is necessary so that we have easy access to
    // mutable strings.
    std::vector<std::string> args;
    args.reserve(argc);
    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    parse(args);
}

inline void CmdLine::parse(std::vector<std::string>& args) {
    bool shouldExit = false;
    int estat = 0;

    try {
        m_progName = args.front();
        args.erase(args.begin());

        int requiredCount = 0;

        for (std::size_t i = 0; i < args.size(); ++i) {
            bool matched = false;
            for (auto& arg : m_argList) {
                if (arg->processArg(i, args)) {
                    requiredCount += m_xorHandler.check(arg);
                    matched = true;
                    break;
                }
            }

            // checks to see if the argument is an empty combined
            // switch and if so, then we've actually matched it
            if (!matched && _emptyCombined(args[i])) {
                matched = true;
            }

            if (!matched && !Arg::ignoreRest()) {
                throw(CmdLineParseException("Couldn't find match "
                                            "for argument",
                    args[i]));
            }
        }

        if (requiredCount < m_numRequired) {
            missingArgsException();
        }

        if (requiredCount > m_numRequired) {
            throw(CmdLineParseException("Too many arguments!"));
        }

    } catch (ArgException& e) {
        // If we're not handling the exceptions, rethrow.
        if (!m_handleExceptions) {
            throw;
        }

        try {
            m_output->failure(*this, e);
        } catch (ExitException& ee) {
            estat = ee.getExitStatus();
            shouldExit = true;
        }
    } catch (ExitException& ee) {
        // If we're not handling the exceptions, rethrow.
        if (!m_handleExceptions) {
            throw;
        }

        estat = ee.getExitStatus();
        shouldExit = true;
    }

    if (shouldExit) {
        exit(estat);
    }
}

inline bool CmdLine::_emptyCombined(const std::string& s) {
    if (s.length() > 0 && s[0] != Arg::flagStartChar()) {
        return false;
    }

    for (int i = 1; i < s.length(); i++) {
        if (s[i] != Arg::blankChar()) {
            return false;
        }
    }

    return true;
}

inline void CmdLine::missingArgsException() const {
    int count = 0;

    std::string missingArgList;
    for (auto arg : m_argList) {
        if (arg->isRequired() && !arg->isSet()) {
            missingArgList += arg->getName();
            missingArgList += ", ";
            ++count;
        }
    }
    missingArgList = missingArgList.substr(0, missingArgList.length() - 2);

    std::string msg;
    if (count > 1) {
        msg = "Required arguments missing: ";
    } else {
        msg = "Required argument missing: ";
    }

    msg += missingArgList;

    throw(CmdLineParseException(msg));
}

inline const CmdLineOutput& CmdLine::getOutput() const {
    return *m_output;
}

inline void CmdLine::setOutput(CmdLineOutput* co) {
    m_userSetOutput = true;
    m_output = std::unique_ptr<CmdLineOutput>(co);
}

inline const std::string& CmdLine::getVersion() const {
    return m_version;
}

inline const std::string& CmdLine::getProgramName() const {
    return m_progName;
}

inline const std::vector<Arg*>& CmdLine::getArgList() const {
    return m_argList;
}

inline const XorHandler& CmdLine::getXorHandler() const {
    return m_xorHandler;
}

inline char CmdLine::getDelimiter() const {
    return m_delimiter;
}

inline const std::string& CmdLine::getMessage() const {
    return m_message;
}

inline bool CmdLine::hasHelpAndVersion() const {
    return m_helpAndVersion;
}

inline void CmdLine::setExceptionHandling(const bool state) {
    m_handleExceptions = state;
}

inline bool CmdLine::getExceptionHandling() const {
    return m_handleExceptions;
}

inline void CmdLine::reset() {
    for (auto& it : m_argList) {
        it->reset();
    }

    m_progName.clear();
}

///////////////////////////////////////////////////////////////////////////////
//End CmdLine.cpp
///////////////////////////////////////////////////////////////////////////////

} //namespace TCLAP
#endif
