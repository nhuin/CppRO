
/****************************************************************************** 
 * 
 *  file:  XorHandler.h
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

#ifndef TCLAP_XORHANDLER_H
#define TCLAP_XORHANDLER_H

#include "Arg.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace TCLAP {

/**
 * This class handles lists of Arg's that are to be XOR'd on the command
 * line.  This is used by CmdLine and you shouldn't ever use it.
 */
class XorHandler {
  protected:
    /**
		 * The list of of lists of Arg's to be or'd together.
		 */
    std::vector<std::vector<Arg*>> _orList = std::vector<std::vector<Arg*>>();

  public:
    /**
		 * Constructor.  Does nothing.
		 */
    XorHandler() = default;

    /**
		 * Add a list of Arg*'s that will be orred together.
		 * \param ors - list of Arg* that will be xor'd.
		 */
    void add(const std::vector<Arg*>& ors);

    /**
		 * Checks whether the specified Arg is in one of the xor lists and
		 * if it does match one, returns the size of the xor list that the
		 * Arg matched.  If the Arg matches, then it also sets the rest of
		 * the Arg's in the list. You shouldn't use this.  
		 * \param a - The Arg to be checked.
		 */
    std::size_t check(const Arg* a);

    /**
		 * Returns the XOR specific short usage.
		 */
    std::string shortUsage();

    /**
		 * Prints the XOR specific long usage.
		 * \param os - Stream to print to.
		 */
    void printLongUsage(std::ostream& os);

    /**
		 * Simply checks whether the Arg is contained in one of the arg
		 * lists.
		 * \param a - The Arg to be checked.
		 */
    bool contains(const Arg* a) const;

    const std::vector<std::vector<Arg*>>& getXorList() const;
};

//////////////////////////////////////////////////////////////////////
//BEGIN XOR.cpp
//////////////////////////////////////////////////////////////////////
inline void XorHandler::add(const std::vector<Arg*>& ors) {
    _orList.push_back(ors);
}

inline std::size_t XorHandler::check(const Arg* a) {
    // iterate over each XOR list
    for (auto& list : _orList) {
        // for (std::size_t i = 0; i < _orList.size(); i++) {
        // if the XOR list contains the arg..
        auto ait = std::find(list.begin(),
            list.end(), a);
        if (ait != list.end()) {
            // first check to see if a mutually exclusive switch
            // has not already been set
            for (auto& it : list) {
                if (a != it && it->isSet()) {
                    throw(CmdLineParseException(
                        "Mutually exclusive argument already set!",
                        it->toString()));
                }
            }

            // go through and set each arg that is not a
            for (auto& it : list) {
                if (a != it) {
                    it->xorSet();
                }
            }

            // return the number of required args that have now been set
            if ((*ait)->allowMore()) {
                return 0;
            }
            return list.size();
        }
    }

    if (a->isRequired()) {
        return 1;
    }
    return 0;
}

inline bool XorHandler::contains(const Arg* a) const {
    for (const auto& orArgs : _orList) {
        for (auto arg : orArgs) {
            if (a == arg) {
                return true;
            }
        }
    }
    return false;
}

inline const std::vector<std::vector<Arg*>>& XorHandler::getXorList() const {
    return _orList;
}

//////////////////////////////////////////////////////////////////////
//END XOR.cpp
//////////////////////////////////////////////////////////////////////

} //namespace TCLAP

#endif
